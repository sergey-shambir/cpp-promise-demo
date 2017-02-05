// libpromisewin32.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "AsioThreadPool.h"
#include "AsioEventLoop.h"
#include "Win32EventLoop.h"
#include <iostream>
#include <thread>

void UseThreadPool(isprom::IDispatcher &pool)
{
	auto makePrintFn = [](unsigned value) {
		return [value] {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cerr << value << std::endl;
		};
	};

	pool.Post([] {
		throw std::runtime_error("Bad task called: it should always throw");
	});
    for (unsigned i = 1; i < 42; ++i)
	{
		pool.Post(makePrintFn(i));
	}
}

int main()
{
#if defined(_WIN32)
	isprom::Win32EventLoop eventLoop;
#else
    isprom::AsioEventLoop eventLoop;
#endif
	isprom::AsioThreadPool pool;

	eventLoop.Post([&] {
		UseThreadPool(pool);
    });
    eventLoop.Post([&] {
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        std::cerr << "Exiting" << std::endl;
        eventLoop.DeferQuit();
    });
    eventLoop.Run();

    return 0;
}

