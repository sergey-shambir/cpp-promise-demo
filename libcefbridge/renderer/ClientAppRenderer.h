// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#pragma once

#include <set>

#include "tests/shared/common/client_app.h"

namespace client {

class IClientAppRendererDelegate
{
public:
	virtual void OnRenderThreadCreated(CefRefPtr<ClientAppRenderer> app, CefRefPtr<CefListValue> extra_info)
	{
		(void)app;
		(void)extra_info;
	}

	virtual void OnWebKitInitialized(CefRefPtr<ClientAppRenderer> app)
	{
		(void)app;
	}

	virtual void OnBrowserCreated(CefRefPtr<ClientAppRenderer> app, CefRefPtr<CefBrowser> browser)
	{
		(void)app;
		(void)browser;
	}

	virtual void OnBrowserDestroyed(CefRefPtr<ClientAppRenderer> app, CefRefPtr<CefBrowser> browser)
	{
		(void)app;
		(void)browser;
	}

	virtual CefRefPtr<CefLoadHandler> GetLoadHandler(CefRefPtr<ClientAppRenderer> app)
	{
		(void)app;
		return nullptr;
	}

	virtual bool OnBeforeNavigation(CefRefPtr<ClientAppRenderer> app,
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		cef_navigation_type_t navigation_type,
		bool is_redirect)
	{
		(void)app;
		(void)browser;
		(void)frame;
		(void)request;
		(void)navigation_type;
		return false;
	}

	virtual void OnContextCreated(CefRefPtr<ClientAppRenderer> app,
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context) 
	{
		(void)app;
		(void)browser;
		(void)frame;
		(void)context;
	}

	virtual void OnContextReleased(CefRefPtr<ClientAppRenderer> app,
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context)
	{
		(void)app;
		(void)browser;
		(void)frame;
		(void)context;
	}

	virtual void OnUncaughtException(CefRefPtr<ClientAppRenderer> app,
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context,
		CefRefPtr<CefV8Exception> exception,
		CefRefPtr<CefV8StackTrace> stackTrace)
	{
		(void)app;
		(void)browser;
		(void)frame;
		(void)context;
		(void)exception;
		(void)stackTrace;
	}

	virtual void OnFocusedNodeChanged(CefRefPtr<ClientAppRenderer> app,
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefDOMNode> node)
	{
		(void)app;
		(void)browser;
		(void)frame;
		(void)node;
	}

	// Called when a process message is received. Return true if the message was
	// handled and should not be passed on to other handlers. Delegates
	// should check for unique message names to avoid interfering with each
	// other.
	virtual bool OnProcessMessageReceived(
		CefRefPtr<ClientAppRenderer> app,
		CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message)
	{
		(void)app;
		(void)browser;
		(void)source_process;
		(void)message;
		return false;
	}
};

// Client app implementation for the renderer process.
class ClientAppRenderer 
	: public ClientApp
	, public CefRenderProcessHandler
{
public:
	using DelegateSet = std::set<CefRefPtr<IClientAppRendererDelegate>>;

	ClientAppRenderer();

private:
	// Creates all of the Delegate objects. Implemented by cefclient in
	// client_app_delegates_renderer.cc
	static void CreateDelegates(DelegateSet& delegates);

	// CefApp methods.
	CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE {
		return this;
	}

	// CefRenderProcessHandler methods.
	void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) OVERRIDE;
	void OnWebKitInitialized() OVERRIDE;
	void OnBrowserCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
	void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) OVERRIDE;
	CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE;
	bool OnBeforeNavigation(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		NavigationType navigation_type,
		bool is_redirect) OVERRIDE;
	void OnContextCreated(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context) OVERRIDE;
	void OnContextReleased(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context) OVERRIDE;
	void OnUncaughtException(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context,
		CefRefPtr<CefV8Exception> exception,
		CefRefPtr<CefV8StackTrace> stackTrace) OVERRIDE;
	void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefDOMNode> node) OVERRIDE;
	bool OnProcessMessageReceived(
		CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message) OVERRIDE;

private:
	// Set of supported Delegates.
	DelegateSet m_delegates;

	IMPLEMENT_REFCOUNTING(ClientAppRenderer);
	DISALLOW_COPY_AND_ASSIGN(ClientAppRenderer);
};

}  // namespace client
