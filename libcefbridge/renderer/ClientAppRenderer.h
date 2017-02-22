// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#pragma once

#include <set>

#include "../common/ClientApp.h"

namespace cefbridge
{

class ClientAppRenderer;

class IClientAppRendererDelegate
{
public:
    virtual void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info)
    {
		(void)extra_info;
	}

    virtual void OnWebKitInitialized()
    {
	}

    virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser)
    {
		(void)browser;
	}

    virtual void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser)
    {
		(void)browser;
    }

    virtual bool OnBeforeNavigation(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		cef_navigation_type_t navigation_type,
		bool is_redirect)
    {
		(void)browser;
		(void)frame;
		(void)request;
		(void)navigation_type;
		return false;
	}

    virtual void OnContextCreated(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context) 
    {
		(void)browser;
		(void)frame;
		(void)context;
	}

    virtual void OnContextReleased(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context)
    {
		(void)browser;
		(void)frame;
		(void)context;
	}

    virtual void OnUncaughtException(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context,
		CefRefPtr<CefV8Exception> exception,
		CefRefPtr<CefV8StackTrace> stackTrace)
    {
		(void)browser;
		(void)frame;
		(void)context;
		(void)exception;
		(void)stackTrace;
	}

    virtual void OnFocusedNodeChanged(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefDOMNode> node)
    {
		(void)browser;
		(void)frame;
		(void)node;
	}

	// Called when a process message is received. Return true if the message was
	// handled and should not be passed on to other handlers. Delegates
	// should check for unique message names to avoid interfering with each
	// other.
    virtual bool OnProcessMessageReceived(
		CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message)
    {
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
	ClientAppRenderer();

    void SetDelegate(IClientAppRendererDelegate *delegate);

private:
	// CefApp methods.
    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override {
		return this;
	}

	// CefRenderProcessHandler methods.
    void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) override;
    void OnWebKitInitialized() override;
    void OnBrowserCreated(CefRefPtr<CefBrowser> browser) override;
    void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) override;
	bool OnBeforeNavigation(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		NavigationType navigation_type,
        bool is_redirect) override;
	void OnContextCreated(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
        CefRefPtr<CefV8Context> context) override;
	void OnContextReleased(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
        CefRefPtr<CefV8Context> context) override;
	void OnUncaughtException(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context,
		CefRefPtr<CefV8Exception> exception,
        CefRefPtr<CefV8StackTrace> stackTrace) override;
	void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
        CefRefPtr<CefDOMNode> node) override;
	bool OnProcessMessageReceived(
		CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message) override;

private:
	// Set of supported Delegates.
    IClientAppRendererDelegate *m_delegate = nullptr;

	IMPLEMENT_REFCOUNTING(ClientAppRenderer);
	DISALLOW_COPY_AND_ASSIGN(ClientAppRenderer);
};

}  // namespace client
