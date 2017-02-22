// libpromisewin32.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MainDispatcher.h"
#include <iostream>
#include <thread>


void PrintSync(const char message[])
{
	static std::mutex printMutex;
    std::lock_guard<std::mutex> lock(printMutex);
    const auto id = std::this_thread::get_id();
    std::cerr << "Doing '" << message << "' on thread " << id << std::endl;
}

void Demonstrate(MainDispatcher &dispatcher)
{
    auto p1 = dispatcher.DoOnBackground([] {
        PrintSync("p1 procedure");
        return 42;
    });
    auto p2 = dispatcher.DoOnBackground([] {
        PrintSync("p2 procedure");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        return 38;
    });
    auto p3 = dispatcher.DoOnBackground([] {
        PrintSync("p3 procedure");
        return 46;
    });
    p3.Then([p2, p1, &dispatcher](auto value) {
        PrintSync("p3 then");
        std::cerr << "p3 value = " << value << std::endl;
        p2.Then([p1, &dispatcher](auto value) {
            PrintSync("p2 then");
            std::cerr << "p2 value = " << value << std::endl;
            p1.Then([&dispatcher](auto value) {
                PrintSync("p1 then");
                std::cerr << "p1 value = " << value << std::endl;
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

