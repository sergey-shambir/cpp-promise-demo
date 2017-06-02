#include "stdafx.h"
#include "MainDispatcher.h"
#include <iostream>
#include <thread>

class Demo
{
public:
	static const int TASK_WEIGHT = 1; // always 1.

	Demo(MainDispatcher& dispatcher)
		: m_dispatcher(dispatcher)
	{
		m_tasksCounter = 0;
	}

	void DemonstratePromise()
	{
		OnTaskAdded();
		auto promise1 = m_dispatcher.DoOnBackground([this] {
			PrintSync("promise1 procedure");
			return 42;
		});
		auto promise2 = m_dispatcher.DoOnBackground([this] {
			PrintSync("promise2 procedure");
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			return 38;
		});
		auto promise3 = m_dispatcher.DoOnBackground([this] {
			PrintSync("promise3 procedure");
			return 46;
		});
		promise3.Then([promise2, promise1, this](auto value) {
			PrintSync("promise3 then with value: " + std::to_string(value));
			promise2.Then([promise1, this](auto value) {
				PrintSync("promise2 then with value: " + std::to_string(value));
				promise1.Then([this](auto value) {
					PrintSync("promise1 then with value: " + std::to_string(value));
					OnTaskDone();
				});
			});
		});
	}

	void OnTaskAdded()
	{
		m_tasksCounter.fetch_add(TASK_WEIGHT);
	}

	void OnTaskDone()
	{
		const int taskRemained = m_tasksCounter.fetch_sub(TASK_WEIGHT);
		if (taskRemained - TASK_WEIGHT == 0)
		{
			m_dispatcher.QuitMainLoop();
		}
	}

	void DemonstrateContinuation()
	{
		// Продолжение на фоновом потоке
		{
			OnTaskAdded();
			auto numPromise = m_dispatcher.DoOnBackground([this] {
				std::this_thread::sleep_for(std::chrono::milliseconds(300));
				return 14;
			});
			auto stringPromise = numPromise.ThenDo([this](int value) {
				PrintSync("converting int to string #1: " + std::to_string(value));
				return std::to_string(value);
			});
			stringPromise.Then([this](auto value) {
				assert(!value.empty());
				PrintSync("stringPromise #1 then with value: " + value);
				OnTaskDone();
			});
		}

		// Продолжение на основном потоке
		{
			OnTaskAdded();
			auto numPromise = m_dispatcher.DoOnBackground([this] {
				std::this_thread::sleep_for(std::chrono::milliseconds(300));
				return 25;
			});
			auto stringPromise = numPromise.ThenDo([this](int value) {
				PrintSync("converting int to string #2: " + std::to_string(value));
				return std::to_string(value);
			});
			stringPromise.Then([this](auto value) {
				assert(!value.empty());
				PrintSync("stringPromise #2 then with value: " + value);
				OnTaskDone();
			});
		}

		// Продолжение с обработкой исключения
		{
			OnTaskAdded();
			auto throwPromise = m_dispatcher.DoOnBackground([this] {
				std::this_thread::sleep_for(std::chrono::milliseconds(300));
				throw std::runtime_error("throw promise throws exception");
				return 14;
			});
			auto stringPromise = throwPromise.ThenDo([this](int value) {
                PrintSync("ERROR - should never convert int to string #3: " + std::to_string(value));
                return std::to_string(value); }, [this](const std::exception_ptr& exception) {
                try
                {
                    std::rethrow_exception(exception);
                }
                catch (const std::exception &ex)
                {
                    PrintSync(std::string("got exception: ") + ex.what());
                }
                return std::string("we got exception!"); });
			stringPromise.Then([this](auto value) {
				assert(!value.empty());
				PrintSync("stringPromise #2 then with value: " + value);
				OnTaskDone();
			});
		}
	}

private:
	void PrintSync(const std::string& message) const
	{
		static std::mutex printMutex;
		std::lock_guard<std::mutex> lock(printMutex);
		const auto id = std::this_thread::get_id();
		std::cerr << message << " on thread " << id << std::endl;
	}

	MainDispatcher& m_dispatcher;
	std::atomic<int> m_tasksCounter;
};

int main()
{
	MainDispatcher dispatcher;

	Demo demo(dispatcher);
	dispatcher.DoOnMainThread([&] {
		demo.DemonstratePromise();
		demo.DemonstrateContinuation();
	});

	dispatcher.EnterMainLoop();

	return 0;
}
