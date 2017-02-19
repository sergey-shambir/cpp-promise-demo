#pragma once

#include "include/cef_app.h"
#include "include/wrapper/cef_message_router.h"
#include <set>
#include <memory>

namespace cefbridge
{

class IMessageRouterBrowserSide
{
public:
	using HandlerSharedPtr = std::shared_ptr<CefMessageRouterBrowserSide::Handler>;

	virtual ~IMessageRouterBrowserSide() = default;

	virtual void AddHandler(const HandlerSharedPtr &handler) = 0;
	virtual bool RemoveHandler(const HandlerSharedPtr &handler) = 0;

	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) = 0;
	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) = 0;
	virtual void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser) = 0;
	virtual void OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) = 0;
};

class MessageRouterBrowserSide : public IMessageRouterBrowserSide
{
protected:
	MessageRouterBrowserSide();
	~MessageRouterBrowserSide();

	void AddHandler(const HandlerSharedPtr &handler) override;
	bool RemoveHandler(const HandlerSharedPtr &handler) override;

	void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;
	bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;
	void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser) override;
	void OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) override;

private:
	using HandlerList = std::set<HandlerSharedPtr>;

	HandlerList m_messageHandlers;

	// Handles the browser side of query routing. The renderer side is handled
	// in client_renderer.cpp.
	CefRefPtr<CefMessageRouterBrowserSide> m_messageRouter;
};

}
