// libpromisewin32.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MainDispatcher.h"
#include <iostream>
#include <thread>

std::mutex g_printJobMutex;

void PrintJobThreadId(const char job[])
{
    std::lock_guard<std::mutex> lock(g_printJobMutex);
    const auto id = std::this_thread::get_id();
    std::cerr << "Doing '" << job << "' on thread " << id << std::endl;
}

void Demonstrate(MainDispatcher &dispatcher)
{
    auto p1 = dispatcher.DoOnBackground([] {
        PrintJobThreadId("p1 procedure");
        return std::make_unique<int>(42);
    });
    auto p2 = dispatcher.DoOnBackground([] {
        PrintJobThreadId("p2 procedure");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        return std::make_unique<int>(38);
    });
    auto p3 = dispatcher.DoOnBackground([] {
        PrintJobThreadId("p3 procedure");
        return std::make_shared<int>(46);
    });
    p3.Then([p2, p1, &dispatcher](auto value) {
        PrintJobThreadId("p3 then");
        std::cerr << "p3 value = " << *value << std::endl;
        p2.Then([p1, &dispatcher](auto value) {
            PrintJobThreadId("p2 then");
            std::cerr << "p2 value = " << *value << std::endl;
            p1.Then([&dispatcher](auto value) {
                PrintJobThreadId("p1 then");
                std::cerr << "p1 value = " << *value << std::endl;
                dispatcher.QuitMainLoop();
            });
        });
    });
}

int main()
{
    MainDispatcher dispatcher;

    dispatcher.DoOnMainThread([&] {
        Demonstrate(dispatcher);
    });

    dispatcher.EnterMainLoop();

    return 0;
}

