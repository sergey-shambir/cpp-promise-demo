#pragma once
#include "../platform/IDispatcher.h"
#include "Promise.h"

namespace isc
{

class PromiseFactory
{
public:
	using IDispatcherPtr = isc::IDispatcherPtr;

	/// @param callbackDispatcher - callback operation dispatcher
	explicit PromiseFactory(const IDispatcherPtr& callbackDispatcher)
		: m_callbackDispatcher(callbackDispatcher)
	{
	}

	template<class ValueType>
	decltype(auto) MakeReadyPromise(ValueType value)
	{
		auto promiseObj = std::make_shared<PromiseObject<ValueType>>(m_callbackDispatcher);
		promiseObj->Resolve(value);

		return Promise<ValueType>(promiseObj);
	}

	template<class ValueType>
	decltype(auto) MakePromiseObject()
	{
		return std::make_shared<PromiseObject<ValueType>>(m_callbackDispatcher);
	}

	template<class ValueType>
	decltype(auto) MakeRejectedPromise(const std::exception_ptr& ex)
	{
		auto promiseObj = std::make_shared<PromiseObject<ValueType>>(m_callbackDispatcher);
		promiseObj->Reject(ex);

		return Promise<ValueType>(promiseObj);
	}

private:
	IDispatcherPtr m_callbackDispatcher;
};

} // namespace isc
