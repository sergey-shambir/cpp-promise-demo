#pragma once
#include "../platform/Expected.h"
#include "../platform/IDispatcher.h"
#include "Promise.h"
#include "PromiseObject.h"

namespace isc
{
/// @brief Dispatches function call on callDispatcher
/// Takes fn - function without arguments which returns ValueType.
/// Takes promiseObj - promise object which should be fulfilled or rejected.
/// Any returned value or exception used to resolve/reject Promise.
/// Any callbacks for Promise called on callback dispatcher.
/// Returns function result value promise.
template<class Function, class ValueType>
void CallFunctionAsync(
	Function&& fn,
	const std::shared_ptr<PromiseObject<ValueType>>& promiseObj,
	isc::IDispatcher& callDispatcher)
{
	static_assert(std::is_same<ValueType, std::result_of_t<Function()>>::value, "function and promise return different types");
	assert(promiseObj);

	callDispatcher.Dispatch([fn, promiseObj] {
		isc::Expected<ValueType> result;
		try
		{
			auto value = fn();
			static_assert(std::is_same<decltype(value), ValueType>::value, "unexpected internal types mismatch");
			result = std::move(value);
		}
		catch (...)
		{
			result = std::current_exception();
		}
		if (result.HasValue())
		{
			promiseObj->Resolve(result.Get());
		}
		else
		{
			promiseObj->Reject(result.GetException());
		}
	});
}
} // namespace isc
