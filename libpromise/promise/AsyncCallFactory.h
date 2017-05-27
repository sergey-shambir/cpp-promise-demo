#pragma once
#include "CallFunctionAsync.h"

namespace isc
{
class AsyncCallFactory
{
public:
	using IDispatcherPtr = isc::IDispatcherPtr;

	/// @param callDispatcher - call operation dispatcher
	/// @param callbackDispatcher - callback operation dispatcher
	explicit AsyncCallFactory(const IDispatcherPtr& callDispatcher,
		const IDispatcherPtr& callbackDispatcher)
		: m_callDispatcher(callDispatcher)
		, m_callbackDispatcher(callbackDispatcher)
	{
	}

	/// @brief Dispatches function call on call dispatcher
	/// Takes function without arguments which returns value.
	/// Any returned value or exception used to resolve/reject Promise.
	/// Any callbacks for Promise called on callback dispatcher.
	/// Returns function result value promise.
	template<class Function>
	decltype(auto) MakeCallPromise(Function&& fn)
	{
		using ValueType = std::result_of_t<Function()>;
		return Promise<ValueType>(MakeCallPromiseObject(std::forward<Function>(fn)));
	}

	template<class Function>
	decltype(auto) MakeCallPromiseObject(Function&& fn)
	{
		using ValueType = std::result_of_t<Function()>;
		auto promiseObj = std::make_shared<PromiseObject<ValueType>>(m_callbackDispatcher);

		CallFunctionAsync(std::forward<Function>(fn), promiseObj, *m_callDispatcher);

		return promiseObj;
	}

private:
	IDispatcherPtr m_callDispatcher;
	IDispatcherPtr m_callbackDispatcher;
};
} // namespace isc
