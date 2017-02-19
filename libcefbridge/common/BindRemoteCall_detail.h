#pragma once

#include "JavascriptArgumentsAdapter.h"
#include "ReturnValueConverter.h"
#include <functional>

/**
* Function BindRemoteCall transforms C++ function to the std::function
* which accepts CefListValue and returns CefValue.
*/

using RemoteCallback = std::function<CefRefPtr<CefValue>(const CefRefPtr<CefListValue> &list)>;

namespace apputils
{

namespace detail
{
// TODO: <sergey.shambir> replace `ignore_args`, `for_each_in_tuple_impl`,
//   `for_each_in_tuple`, `apply_tuple_impl`, `apply_tuple`
//    with `std::apply` in VS2017.

// Used only for integer sequence unpack
template <typename ...Args>
void ignore_args(Args...)
{
}

// Calls `function` for each index in integer sequence `Is`
template<typename Tuple, typename Function, int... Is>
void for_each_in_tuple_impl(Tuple&& args, Function const& function, std::integer_sequence<size_t, Is...>)
{
	(void)args;
	(void)function;
	ignore_args((function(std::get<Is>(args), Is), 0)...);
}

// Calls `function` for each tuple element
template<typename... Ts, typename Function>
void for_each_in_tuple(std::tuple<Ts...> & params, Function const& function)
{
	for_each_in_tuple_impl(params, function, std::make_integer_sequence<size_t, sizeof...(Ts)>{});
}

// Applies tuple to function using tuple element indexes sequence
template<typename ReturnType, typename Function, typename... Ts, int ...Sequence>
ReturnType apply_tuple_impl(std::tuple<Ts...> & params, Function const& function, std::integer_sequence<size_t, Sequence...>)
{
	(void)params;
	return function(std::get<Sequence>(params) ...);
}

// Applies function to tuple like to arguments list
template<typename ReturnType, typename Function, typename... Ts>
ReturnType apply_tuple(std::tuple<Ts...> & params, Function const& function)
{
	return apply_tuple_impl<ReturnType>(params, function, std::make_integer_sequence<size_t, sizeof...(Ts)>{});
}

// Remove `const&` and other dangerous qualifiers.
template <typename ...Args>
using arguments_tuple = std::tuple<typename std::decay<Args>::type...>;

/////////////////////////////////////////////////////////////////////////////////////////////////
// Function traits keep `std::function<>` typedef for any callable.
// Result of std::bind is not supported.

// For generic types that are functors, delegate to its 'operator()'
// Does not work for result of std::bind.
template <typename T>
struct function_traits
	: public function_traits<decltype(&T::operator())>
{
};

// for pointers to const member function
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const>
{
	typedef std::function<ReturnType(Args...)> f_type;
};

// for pointers to member function
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) >
{
	typedef std::function<ReturnType(Args...)> f_type;
};

// for function pointers
template <typename ReturnType, typename... Args>
struct function_traits<ReturnType(*)(Args...)>
{
	typedef std::function<ReturnType(Args...)> f_type;
};

template <typename Callable>
typename function_traits<Callable>::f_type make_function(Callable callable)
{
	return (typename function_traits<Callable>::f_type)(callable);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Convert std::reference_wrapper<T>, T, T* or some_pointer<T> to T*
// Const qualifier stays preserved.

struct tag_dereferenced_address {};
struct tag_raw_address {};

template <class ClassType, class ReferenceType>
inline ClassType *make_object_pointer_impl(ReferenceType &&reference, tag_dereferenced_address)
{
	return &(*reference);
}

template <class ClassType, class ReferenceType>
inline ClassType *make_object_pointer_impl(ReferenceType &&reference, tag_raw_address)
{
	return &reference;
}

template <class ClassType, class ReferenceType>
inline ClassType *make_object_pointer(ReferenceType &&reference)
{
	using RawClassType = std::decay<ClassType>::type;
	using RawReferenceType = std::decay<ReferenceType>::type;
	using TagType = std::conditional<std::is_same<RawClassType, RawReferenceType>::value, tag_raw_address, tag_dereferenced_address>::type;

	return make_object_pointer_impl<ClassType, ReferenceType>(std::forward<ReferenceType>(reference), TagType());
}

} // namespace detail
} // namespace apputils
