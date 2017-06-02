#pragma once
#include "pain-level.h"
#include "platform/IDispatcher.h"
#include <atomic>
#include <boost/thread/future.hpp>
#include <memory>

class FutureFactory
	: public boost::enable_shared_from_this<FutureFactory>,
	  public boost::executor
{
public:
	FutureFactory(isc::IDispatcher& callDispatcher, isc::IDispatcher& callbackDispatcher);

	template<class Function>
	decltype(auto) MakePromise(Function&& function)
	{
		using ResultType = std::result_of_t<Function()>;
		using PromiseType = boost::promise<ResultType>;
		using FutureType = boost::future<ResultType>;

		auto pPromise = std::make_shared<PromiseType>();
		pPromise->set_executor(boost::executor_ptr_type(shared_from_this()));
		m_callDispatcher.Dispatch([pPromise, function] {
			try
			{
				ResultType type = function();
				pPromise->set_value(std::move(type));
			}
			catch (...)
			{
				pPromise->set_exception(std::current_exception());
			}
		});

		return pPromise->get_future();
	}

private:
	void close() final;
	bool closed() final;
	void submit(work&& closure);
	bool try_executing_one();

	std::atomic_bool m_closed;
	isc::IDispatcher& m_callDispatcher;
	isc::IDispatcher& m_callbackDispatcher;
};
