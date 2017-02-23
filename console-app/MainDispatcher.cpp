#include "stdafx.h"
#include "MainDispatcher.h"

MainDispatcher::MainDispatcher()
    : m_backgroundPromiseFactory(m_pool, m_eventLoop)
{
}

void MainDispatcher::DoOnMainThread(const isprom::Operation &operation)
{
    m_eventLoop.Post(operation);
}


isprom::IDispatcher &MainDispatcher::GetMainThreadDispatcher()
{
    return m_eventLoop;
}


isprom::IDispatcher &MainDispatcher::GetThreadPoolDispatcher()
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
