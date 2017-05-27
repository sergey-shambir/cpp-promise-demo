#include "stdafx.h"
#include "MainDispatcher.h"

MainDispatcher::MainDispatcher()
	: m_pool(std::make_shared<isc::AsioThreadPool>())
#if defined(_WIN32)
	, m_eventLoop(std::make_shared<isc::Win32EventLoop>())
#else
	, m_eventLoop(std::make_shared<isc::AsioEventLoop>())
#endif
	, m_callFactory(m_pool, m_eventLoop)
{
}

void MainDispatcher::DoOnMainThread(const isc::Operation& operation)
{
	m_eventLoop->Dispatch(operation);
}

isc::IDispatcher& MainDispatcher::GetMainThreadDispatcher()
{
	return *m_eventLoop;
}

isc::IDispatcher& MainDispatcher::GetThreadPoolDispatcher()
{
	return *m_pool;
}

void MainDispatcher::EnterMainLoop()
{
	m_eventLoop->Run();
}

void MainDispatcher::QuitMainLoop()
{
	m_eventLoop->DeferQuit();
}
