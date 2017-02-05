// libpromisewin32.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "AsioThreadPool.h"
#include "Win32EventLoop.h"
#include <iostream>
#include <thread>

void UseThreadPool(isprom::IDispatcher &pool)
{
	auto makePrintFn = [](unsigned value) {
		return [value] {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			std::cout << value;
		};
	};

	pool.Post([] {
		throw std::runtime_error("Bad task called: it should always throw");
	});
	for (unsigned i = 1; i < 100; ++i)
	{
		pool.Post(makePrintFn(i));
	}
}

int main()
{
	isprom::Win32EventLoop eventLoop;
	isprom::AsioThreadPool pool;

	eventLoop.Post([&] {
		UseThreadPool(pool);
	});
	eventLoop.Run();

    return 0;
}

