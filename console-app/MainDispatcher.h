#pragma once
#include "Types.h"
#include "platform/AsioThreadPool.h"
#include "platform/AsioEventLoop.h"
#include "platform/Win32EventLoop.h"
#include "promise/PromiseFactory.h"
#include "FutureFactory.h"

class MainDispatcher
{
public:
    MainDispatcher();

    /// Выполняет переданную функцию в фоновом потоке.
    /// Возвращает Promise<T>, где T - тип, возвращаемый переданной функцией.
    /// Колбек у Promise выполняется в основном потоке.
    template <class Function>
    decltype(auto) DoOnBackground(Function && function)
    {
        return m_backgroundPromiseFactory.MakePromise(std::forward<Function>(function));
    }

    /// Выполняет переданную операцию в основном потоке.
    void DoOnMainThread(const isprom::Operation &operation);

    isprom::IDispatcher &GetMainThreadDispatcher();

    isprom::IDispatcher &GetThreadPoolDispatcher();

    /// Входит в основной цикл и исполняет его, пока не будет вызван QuitMainLoop.
    void EnterMainLoop();

    /// Завершает ранее начатое выполнение основного цикла.
    void QuitMainLoop();

private:
#if defined(_WIN32) && 1
    isprom::Win32EventLoop m_eventLoop;
#else
    isprom::AsioEventLoop m_eventLoop;
#endif
    isprom::AsioThreadPool m_pool;
#if OPTION_USE_FUTURE_FACTORY
    isprom::FutureFactory m_backgroundPromiseFactory;
#else
    isprom::PromiseFactory m_backgroundPromiseFactory;
#endif
};
