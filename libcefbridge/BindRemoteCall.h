#pragma once

#include "common/BindRemoteCall_detail.h"

namespace apputils
{

/**
 * Function ApplyCefArguments calls given function and unpacks CefListValue as arguments.
 * Argument type checks is implemented in detail::CJavascriptArgumentsAdapter
 *
 * Function BindRemoteCall transforms C++ function to the std::function
 * which accepts CefListValue and returns CefValue.
*/

namespace detail
{
template <typename R, typename ...Args>
R ApplyCefArgumentsImpl(R(*function)(Args...), const CefRefPtr<CefListValue> & args)
{
	detail::CJavascriptArgumentsAdapter adapter(args);
	detail::arguments_tuple<Args...> typedArgs;

	adapter.CheckArgumentsCount(std::tuple_size<decltype(typedArgs)>::value);
	detail::for_each_in_tuple(typedArgs, adapter);

	return detail::apply_tuple<R>(typedArgs, function);
}

template <typename R, typename ...Args>
R ApplyCefArgumentsImpl(const std::function<R(Args...)> &function, const CefRefPtr<CefListValue> & args)
{
	detail::CJavascriptArgumentsAdapter adapter(args);
	detail::arguments_tuple<Args...> typedArgs;

	adapter.CheckArgumentsCount(std::tuple_size<decltype(typedArgs)>::value);
	detail::for_each_in_tuple(typedArgs, adapter);

	return detail::apply_tuple<R>(typedArgs, function);
}

// For non-void return values.
template <typename ReturnedType, typename ...Args>
RemoteCallback BindRemoteCallImpl(const std::function<ReturnedType(Args...)> &function)
{
	return [function](const CefRefPtr<CefListValue> &list) {
		return ConvertReturnValue(ApplyCefArgumentsImpl(function, list));
	};
}

// For void return values.
template <typename ...Args>
RemoteCallback BindRemoteCallImpl(const std::function<void(Args...)> &function)
{
	return [function](const CefRefPtr<CefListValue> &list) {
		ApplyCefArgumentsImpl(function, list);
		auto pValue = CefValue::Create();
		pValue->SetNull();
		return pValue;
	};
}
}

// For std::function, lambdas or function pointers
template <typename Callable>
void ApplyCefArguments(Callable && callable, const CefRefPtr<CefListValue> & args)
{
	return detail::ApplyCefArgumentsImpl(detail::make_function(callable), args);
}

// For std::function, lambdas or function pointers
template <typename Callable>
RemoteCallback BindRemoteCall(Callable && callable)
{
	return detail::BindRemoteCallImpl(detail::make_function(callable));
}

// for pointers to const member function
template <class ClassType, class ReferenceType, class ReturnType, class... Args>
RemoteCallback BindRemoteCall(ReturnType(ClassType::*method)(Args...)const, ReferenceType &&object)
{
	using ConstClassType = std::add_const<ClassType>::type;

	ConstClassType *pObject = detail::make_object_pointer<ConstClassType, ReferenceType>(std::forward<ReferenceType>(object));
	return BindRemoteCall([=](Args... args) -> ReturnType {
		return (pObject->*method)(args...);
	});
}

// for pointers to member function
template <class ClassType, class ReferenceType, class ReturnType, class... Args>
RemoteCallback BindRemoteCall(ReturnType(ClassType::*method)(Args...), ReferenceType &&object)
{
	ClassType *pObject = detail::make_object_pointer<ClassType, ReferenceType>(std::forward<ReferenceType>(object));
	return BindRemoteCall([=](Args... args) -> ReturnType {
		return (pObject->*method)(args...);
	});
}

}
