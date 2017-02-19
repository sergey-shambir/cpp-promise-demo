#include "../stdafx.h"
#include "MessageRouterBrowserSide.h"

namespace cefbridge
{

MessageRouterBrowserSide::MessageRouterBrowserSide()
{
}

MessageRouterBrowserSide::~MessageRouterBrowserSide()
{
}

void MessageRouterBrowserSide::AddHandler(const HandlerSharedPtr &handler)
{
	if (m_messageHandlers.find(handler) == m_messageHandlers.end())
	{
		m_messageHandlers.insert(handler);
		m_messageRouter->AddHandler(handler.get(), false);
	}
}

bool MessageRouterBrowserSide::RemoveHandler(const HandlerSharedPtr &handler)
{
	auto foundIt = m_messageHandlers.find(handler);
	if (foundIt != m_messageHandlers.end())
	{
		m_messageRouter->RemoveHandler(handler.get());
		m_messageHandlers.erase(foundIt);
		return true;
	}
	return false;
}

void MessageRouterBrowserSide::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
	m_messageRouter->OnBeforeClose(browser);
}

bool MessageRouterBrowserSide::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
	return m_messageRouter->OnProcessMessageReceived(browser, source_process, message);
}

void MessageRouterBrowserSide::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser)
{
	m_messageRouter->OnRenderProcessTerminated(browser);
}

void MessageRouterBrowserSide::OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
{
	m_messageRouter->OnBeforeBrowse(browser, frame);
}

}
