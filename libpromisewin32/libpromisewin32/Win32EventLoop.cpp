#include "stdafx.h"
#include "Win32EventLoop.h"
#include "Win32NonBlockingDispatcher.h"

// WTL includes
#define NOMINMAX
#include <Windows.h>
#include <atlbase.h>
#include <atlapp.h>


namespace isprom
{
struct Win32EventLoop::Impl
{
	void Run()
	{
		WTL::CMessageLoop loop;
		loop.Run();
	}

	void Post(const Operation &operation)
	{
		m_dispatcher.Post(operation);
	}

private:
	Win32NonBlockingDispatcher m_dispatcher;
};

Win32EventLoop::Win32EventLoop()
	: m_impl(std::make_unique<Impl>())
{
}

Win32EventLoop::~Win32EventLoop()
{
}

void Win32EventLoop::Run()
{
	m_impl->Run();
}

void Win32EventLoop::Post(const Operation &operation)
{
	m_impl->Post(operation);
}

}
