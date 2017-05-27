#pragma once
#include <stdexcept>
#include <variant>

namespace isc
{

namespace detail
{
[[noreturn]] void RethrowResultException(const std::exception_ptr& ex);
[[noreturn]] void ThrowInvalidResult();
[[noreturn]] void ThrowResultHasNoException();
std::exception_ptr VerifyResultException(std::exception_ptr&& ex);
} // namespace detail

/// @brief Keeps either value or exception_ptr
template<class Value>
class Expected
{
public:
	Expected() = default;

	explicit Expected(const Value& value)
		: m_storage(value)
	{
	}

	explicit Expected(Value&& value)
		: m_storage(std::move(value))
	{
	}

	explicit Expected(const std::exception_ptr& ex)
		: m_storage(ex)
	{
	}

	explicit Expected(std::exception_ptr&& ex)
		: m_storage(std::move(ex))
	{
	}

	Expected& operator=(const Value& value)
	{
		m_storage = value;
		return *this;
	}

	Expected& operator=(Value&& value)
	{
		m_storage = std::move(value);
		return *this;
	}

	Expected& operator=(std::exception_ptr&& ex)
	{
		m_storage = std::move(ex);
		return *this;
	}

	Expected& operator=(const std::exception_ptr& ex)
	{
		m_storage = ex;
		return *this;
	}

	explicit operator bool() const noexcept
	{
		return HasValue();
	}

	Value Get()
	{
		if (!HasValue())
		{
			detail::RethrowResultException(GetException());
		}
		Value value = std::move_if_noexcept(std::get<Value>(m_storage));
		m_storage = std::exception_ptr();
		return value;
	}

	std::exception_ptr GetException()
	{
		if (HasValue())
		{
			detail::ThrowResultHasNoException();
		}
		return detail::VerifyResultException(std::move(std::get<std::exception_ptr>(m_storage)));
	}

	bool HasValue() const noexcept
	{
		return (m_storage.index() == ValueIndex);
	}

private:
	// ValueTypeWhich must match Value index in variant
	using Storage = std::variant<std::exception_ptr, Value>;

	static constexpr int ValueIndex = 1;

	// Made exception_ptr first to be default constructable.
	Storage m_storage;
};
} // namespace isc
