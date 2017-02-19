// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "simple_handler.h"

#include <sstream>
#include <string>

#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

#if defined(__linux__)
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#endif

namespace {

SimpleHandler* g_instance = NULL;

}  // namespace

SimpleHandler::SimpleHandler(bool use_views)
    : m_useViews(use_views),
    m_isClosing(false) {
    DCHECK(!g_instance);
    g_instance = this;
}

SimpleHandler::~SimpleHandler()
{
    g_instance = NULL;
}

// static
SimpleHandler* SimpleHandler::GetInstance()
{
    return g_instance;
}


CefRefPtr<CefDisplayHandler> SimpleHandler::GetDisplayHandler()
{
	return this;
}

CefRefPtr<CefLifeSpanHandler> SimpleHandler::GetLifeSpanHandler()
{
	return this;
}

CefRefPtr<CefLoadHandler> SimpleHandler::GetLoadHandler()
{
	return this;
}

void SimpleHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
    const CefString& title) {
    CEF_REQUIRE_UI_THREAD();

    if (m_useViews) {
        // Set the title of the window using the Views framework.
        CefRefPtr<CefBrowserView> browser_view =
            CefBrowserView::GetForBrowser(browser);
        if (browser_view) {
            CefRefPtr<CefWindow> window = browser_view->GetWindow();
            if (window)
                window->SetTitle(title);
        }
    }
    else {
        // Set the title of the window using platform APIs.
        PlatformTitleChange(browser, title);
    }
}

void SimpleHandler::PlatformTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
	CEF_REQUIRE_UI_THREAD();

#if defined(_WIN32)
	CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
	::SetWindowText(hwnd, std::wstring(title).c_str());
#endif

#if defined(__linux__)
	std::string titleStr(title);

    // Retrieve the X11 display shared with Chromium.
    ::Display* display = cef_get_xdisplay();
    DCHECK(display);

    // Retrieve the X11 window handle for the browser.
    ::Window window = browser->GetHost()->GetWindowHandle();
    DCHECK(window != kNullWindowHandle);

    // Retrieve the atoms required by the below XChangeProperty call.
    const char* kAtoms[] = {
        "_NET_WM_NAME",
        "UTF8_STRING"
    };
    Atom atoms[2];
    int result = XInternAtoms(display, const_cast<char**>(kAtoms), 2, false,
        atoms);
    if (!result)
        NOTREACHED();

    // Set the window title.
    XChangeProperty(display,
        window,
        atoms[0],
        atoms[1],
        8,
        PropModeReplace,
        reinterpret_cast<const unsigned char*>(titleStr.c_str()),
        titleStr.size());

    // TODO(erg): This is technically wrong. So XStoreName and friends expect
    // this in Host Portable Character Encoding instead of UTF-8, which I believe
    // is Compound Text. This shouldn't matter 90% of the time since this is the
    // fallback to the UTF8 property above.
    XStoreName(display, browser->GetHost()->GetWindowHandle(), titleStr.c_str());
#endif
}

void SimpleHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();

    // Add to the list of existing browsers.
    m_browserList.push_back(browser);

	if (!m_messageRouter)
	{
		CefMessageRouterConfig config;
		config.js_query_function = "cefQuery";
		config.js_cancel_function = "cefQueryCancel";
		m_messageRouter = CefMessageRouterBrowserSide::Create(config);
	}
}

bool SimpleHandler::DoClose(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();

    // Closing the main window requires special handling. See the DoClose()
    // documentation in the CEF header for a detailed destription of this
    // process.
    if (m_browserList.size() == 1) {
        // Set a flag to indicate that the window close should be allowed.
        m_isClosing = true;
    }

    // Allow the close. For windowed browsers this will result in the OS close
    // event being sent.
    return false;
}

void SimpleHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();

    // Remove from the list of existing browsers.
    BrowserList::iterator bit = m_browserList.begin();
    for (; bit != m_browserList.end(); ++bit) {
        if ((*bit)->IsSame(browser)) {
            m_browserList.erase(bit);
            break;
        }
    }

    if (m_browserList.empty()) {
		m_messageRouter->RemoveHandler();
		m_messageRouter = nullptr;

        // All browser windows have closed. Quit the application message loop.
        CefQuitMessageLoop();
    }
}

void SimpleHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    ErrorCode errorCode,
    const CefString& errorText,
    const CefString& failedUrl) {
    CEF_REQUIRE_UI_THREAD();

    // Don't display an error for downloaded files.
    if (errorCode == ERR_ABORTED)
        return;

    // Display a load error message.
    std::stringstream ss;
    ss << "<html><body bgcolor=\"white\">"
        "<h2>Failed to load URL " << std::string(failedUrl) <<
        " with error " << std::string(errorText) << " (" << errorCode <<
        ").</h2></body></html>";
    frame->LoadString(ss.str(), failedUrl);
}

void SimpleHandler::CloseAllBrowsers(bool force_close)
{
    if (!CefCurrentlyOn(TID_UI)) {
        // Execute on the UI thread.
        CefPostTask(TID_UI, base::Bind(&SimpleHandler::CloseAllBrowsers, this, force_close));
        return;
    }

	for (auto &browser : m_browserList)
	{
		browser->GetHost()->CloseBrowser(force_close);
	}
}


bool SimpleHandler::IsClosing() const
{
	return m_isClosing;
}

bool SimpleHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
#error TODO
}
