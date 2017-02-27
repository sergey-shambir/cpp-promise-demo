#include "stdafx.h"
#include "MainDispatcher.h"
#include <iostream>
#include <thread>


class Demo
{
public:
    static const int TASK_WEIGHT = 1; // always 1.

    Demo(MainDispatcher &dispatcher)
        : m_dispatcher(dispatcher)
    {
        m_tasksCounter = 0;
    }

    // Подводный камень: колбек от then вызывается в новом потоке,
    //  который boost создал специально для вызова.
    // Это крайне дорого для такой простой операци!
    void WannaCallbackOnNewThread()
    {
        // .. создаём boost::promise и получаем от него future
        auto promise = std::make_shared<boost::promise<int>>();
        m_dispatcher.GetThreadPoolDispatcher().Post([this, promise] {
            std::this_thread::sleep(std::chrono::milliseconds(300));
            PrintSync("finished async task");
            return 14;
        });
        auto future = promise.get_future();

        PrintSync("called then method");
        future.then([&](future<string> oldFuture) {
            PrintSync("called then callback");
            dispatch.QuitMainLoop();
        });
    }

    void WannaNeverCallThenCallback()
    {
        // .. создаём boost::promise и получаем от него future
        auto promise = std::make_shared<boost::promise<int>>();
        m_dispatcher.GetThreadPoolDispatcher().Post([this, promise] {
            std::this_thread::sleep(std::chrono::milliseconds(300));
            PrintSync("finished async task");
            return 14;
        });
        auto future = promise.get_future();

        PrintSync("called then method");
        future.then(launch::deferred, [&](future<string> oldFuture) {
            PrintSync("called then callback");
            dispatch.QuitMainLoop();
        });
    }

    void WannaUseEmptyExecutor()
    {
        // .. создаём boost::promise и получаем от него future
        auto promise = std::make_shared<boost::promise<int>>();
        m_dispatcher.GetThreadPoolDispatcher().Post([this, promise] {
            std::this_thread::sleep(std::chrono::milliseconds(300));
            PrintSync("finished async task");
            return 14;
        });
        auto future = promise.get_future();

        PrintSync("called then method");
        future.then(launch::executor, [&](future<string> oldFuture) {
            PrintSync("called then callback");
            dispatch.QuitMainLoop();
        });
    }

    void WannaWaitForever()
    {
        m_dispatcher.DoOnBackground([] {
            return 42;
        });

        PrintSync("called then method");
        auto f2 = future.then(launch::executor, [&](future<string> oldFuture) {
            PrintSync("called then callback");
            dispatch.QuitMainLoop();
        });
        // Вызов wait приводит к тому, что выполнение колбека, добавленного
        //  через then, не происходит никогда.
        // Значит, future икогда не получает результат.
        // Значит, wait никогда не закончится.
        f2.wait();
    }

    void NoPainNoGame()
    {
        m_dispatcher.DoOnBackground([] {
            return 42;
        });

        PrintSync("called then method");
        auto f2 = future.then(launch::executor, [&](future<string> oldFuture) {
            PrintSync("called then callback");
            dispatch.QuitMainLoop();
        });

        // Теперь мы не вызываем wait. Но его легко вызовет какой-нибудь
        //  из программистов в команде.
    }

private:
    void PrintSync(const std::string &message) const
    {
        static std::mutex printMutex;
        std::lock_guard<std::mutex> lock(printMutex);
        const auto id = std::this_thread::get_id();
        std::cerr << message << "' on thread " << id << std::endl;
    }

    MainDispatcher &m_dispatcher;
    std::atomic<int> m_tasksCounter;
};


int main()
{
    MainDispatcher dispatcher;

    Demo demo(dispatcher);
    dispatcher.DoOnMainThread([&] {
        const int level = OPTION_PAIN_LEVEL;
        switch (level)
        {
            case 1:
                demo.WannaCallbackOnNewThread();
                break;
            case 2:
                demo.WannaNeverCallThenCallback();
                break;
            case 3:
                demo.WannaUseEmptyExecutor();
                break;
            case 4:
                demo.WannaWaitForever();
                break;
            default:
                demo.NoPainNoGame();
                break;
        }
    });

    dispatcher.EnterMainLoop();

    return 0;
}
