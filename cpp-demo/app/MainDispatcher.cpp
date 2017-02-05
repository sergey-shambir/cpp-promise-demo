#include "stdafx.h"
#include "MainDispatcher.h"

MainDispatcher::MainDispatcher()
    : m_backgroundPromiseFactory(m_eventLoop, m_pool)
{
}

void MainDispatcher::DoOnMainThread(const isprom::Operation &operation)
{
    m_eventLoop.Post(operation);
}

void MainDispatcher::EnterMainLoop()
{
    m_eventLoop.Run();
}

void MainDispatcher::QuitMainLoop()
{
    m_eventLoop.DeferQuit();
}
