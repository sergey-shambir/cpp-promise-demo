#pragma once
#include "FutureFactory.h"
#include "Types.h"
#include "platform/AsioEventLoop.h"
#include "platform/AsioThreadPool.h"
#include "platform/Win32EventLoop.h"
#include <boost/noncopyable.hpp>

class MainDispatcher : public boost::noncopyable
{
public:
    MainDispatcher();

    /// Выполняет переданную функцию в фоновом потоке.
    /// Возвращает Promise<T>, где T - тип, возвращаемый переданной функцией.
    /// Колбек у Promise выполняется в основном потоке.
    template<class Function>
    decltype(auto) DoOnBackground(Function &&function)
    {
        return m_backgroundFutureFactory->MakePromise(std::forward<Function>(function));
    }

    /// Выполняет переданную операцию в основном потоке.
    void DoOnMainThread(const isc::Operation &operation);

    isc::IDispatcher &GetMainThreadDispatcher();

    isc::IDispatcher &GetThreadPoolDispatcher();

    /// Входит в основной цикл и исполняет его, пока не будет вызван QuitMainLoop.
    void EnterMainLoop();

    /// Завершает ранее начатое выполнение основного цикла.
    void QuitMainLoop();

private:
    isc::AsioEventLoop m_eventLoop;
    isc::AsioThreadPool m_pool;
    boost::shared_ptr<FutureFactory> m_backgroundFutureFactory;
};
