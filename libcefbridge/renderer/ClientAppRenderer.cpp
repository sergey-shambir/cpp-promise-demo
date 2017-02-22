// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "ClientAppRenderer.h"
#include "include/base/cef_logging.h"

namespace cefbridge
{

ClientAppRenderer::ClientAppRenderer()
{
}

void ClientAppRenderer::SetDelegate(IClientAppRendererDelegate *delegate)
{
    m_delegate = delegate;
}

void ClientAppRenderer::OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info)
{
    m_delegate->OnRenderThreadCreated(extra_info);
}

void ClientAppRenderer::OnWebKitInitialized()
{
    m_delegate->OnWebKitInitialized();
}

void ClientAppRenderer::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
{
    m_delegate->OnBrowserCreated(browser);
}

void ClientAppRenderer::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser)
{
    m_delegate->OnBrowserDestroyed(browser);
}

bool ClientAppRenderer::OnBeforeNavigation(CefRefPtr<CefBrowser> browser,
                                           CefRefPtr<CefFrame> frame,
                                           CefRefPtr<CefRequest> request,
                                           NavigationType navigation_type,
                                           bool is_redirect)
{
    return m_delegate->OnBeforeNavigation(browser, frame, request, navigation_type, is_redirect);
}

void ClientAppRenderer::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                         CefRefPtr<CefFrame> frame,
                                         CefRefPtr<CefV8Context> context)
{
    m_delegate->OnContextCreated(browser, frame, context);
}

void ClientAppRenderer::OnContextReleased(CefRefPtr<CefBrowser> browser,
                                          CefRefPtr<CefFrame> frame,
                                          CefRefPtr<CefV8Context> context)
{
    m_delegate->OnContextReleased(browser, frame, context);
}

void ClientAppRenderer::OnUncaughtException(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context,
    CefRefPtr<CefV8Exception> exception,
    CefRefPtr<CefV8StackTrace> stackTrace)
{
    m_delegate->OnUncaughtException(browser, frame, context, exception, stackTrace);
}

void ClientAppRenderer::OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
                                             CefRefPtr<CefFrame> frame,
                                             CefRefPtr<CefDOMNode> node)
{
    m_delegate->OnFocusedNodeChanged(browser, frame, node);
}

bool ClientAppRenderer::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message)
{
  DCHECK_EQ(source_process, PID_BROWSER);

  return m_delegate->OnProcessMessageReceived(browser, source_process, message);
}

}  // namespace client
