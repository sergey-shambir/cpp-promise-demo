#pragma once

namespace apputils
{
namespace detail
{

template<typename T, typename U, typename... Us>
struct is_any_of
	: std::integral_constant<
		bool,
		std::conditional<
			std::is_same<T, U>::value,
			std::true_type,
	is_any_of<T, Us...>
		>::type::value
	>
{ };

template<typename T, typename U>
struct is_any_of<T, U> : std::is_same<T, U>::type { };

}
}
