#include "stdafx.h"
#include "MainDispatcher.h"

MainDispatcher::MainDispatcher()
	: m_backgroundFutureFactory(boost::make_shared<FutureFactory>(m_pool, m_eventLoop))
{
}

void MainDispatcher::DoOnMainThread(const isc::Operation& operation)
{
	m_eventLoop.Dispatch(operation);
}

isc::IDispatcher& MainDispatcher::GetMainThreadDispatcher()
{
	return m_eventLoop;
}

isc::IDispatcher& MainDispatcher::GetThreadPoolDispatcher()
{
	return m_pool;
}

void MainDispatcher::EnterMainLoop()
{
	m_eventLoop.Run();
}

void MainDispatcher::QuitMainLoop()
{
	m_eventLoop.DeferQuit();
}
