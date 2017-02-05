#pragma once
#include "Types.h"
#include "platform/AsioThreadPool.h"
#include "platform/AsioEventLoop.h"
#include "platform/Win32EventLoop.h"
#include "promise/PromiseFactory.h"

class MainDispatcher
{
public:
    MainDispatcher();

    template <class Function>
    decltype(auto) DoOnBackground(Function && function)
    {
        return m_backgroundPromiseFactory.MakePromise(std::forward<Function>(function));
    }

    void DoOnMainThread(const isprom::Operation &operation);

    void EnterMainLoop();
    void QuitMainLoop();

private:
#if defined(_WIN32)
    isprom::Win32EventLoop m_eventLoop;
#else
    isprom::AsioEventLoop m_eventLoop;
#endif
    isprom::AsioThreadPool m_pool;
    isprom::PromiseFactory m_backgroundPromiseFactory;
};
