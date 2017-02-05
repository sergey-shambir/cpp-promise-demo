// libpromisewin32.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MainDispatcher.h"
#include <iostream>
#include <thread>

void Demonstrate(MainDispatcher &dispatcher)
{
    auto p1 = dispatcher.DoOnBackground([] {
        return 42;
    });
    auto p2 = dispatcher.DoOnBackground([] {
        return 38;
    });
    auto p3 = dispatcher.DoOnBackground([] {
        return 46;
    });
    p3.Then([p2, p1, &dispatcher](int value) {
        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cerr << "p3 value = " << value << std::endl;
        p2.Then([p1, &dispatcher](int value) {
            std::cerr << "p2 value = " << value << std::endl;
            p1.Then([&dispatcher](int value) {
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

