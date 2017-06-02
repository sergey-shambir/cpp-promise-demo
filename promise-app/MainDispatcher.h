#pragma once
#include "Types.h"
#include "platform/AsioEventLoop.h"
#include "platform/AsioThreadPool.h"
#include "platform/Win32EventLoop.h"
#include "promise/AsyncCallFactory.h"

class MainDispatcher
{
public:
	MainDispatcher();

	/// Выполняет переданную функцию в фоновом потоке.
	/// Возвращает Promise<T>, где T - тип, возвращаемый переданной функцией.
	/// Колбек у Promise выполняется в основном потоке.
	template<class Function>
	decltype(auto) DoOnBackground(Function&& function)
	{
		return m_callFactory.MakeCallPromise(std::forward<Function>(function));
	}

	/// Выполняет переданную операцию в основном потоке.
	void DoOnMainThread(const isc::Operation& operation);

	isc::IDispatcher& GetMainThreadDispatcher();

	isc::IDispatcher& GetThreadPoolDispatcher();

	/// Входит в основной цикл и исполняет его, пока не будет вызван QuitMainLoop.
	void EnterMainLoop();

	/// Завершает ранее начатое выполнение основного цикла.
	void QuitMainLoop();

private:
#if defined(_WIN32)
	std::shared_ptr<isc::Win32EventLoop> m_eventLoop;
#else
	std::shared_ptr<isc::AsioEventLoop> m_eventLoop;
#endif
	std::shared_ptr<isc::AsioThreadPool> m_pool;
	isc::AsyncCallFactory m_callFactory;
};
