#include "stdafx.h"
#include "Expected.h"

namespace isc
{
namespace detail
{
void RethrowResultException(const std::exception_ptr& ex)
{
	if (ex)
	{
		std::rethrow_exception(ex);
	}
	ThrowInvalidResult();
}

void ThrowInvalidResult()
{
	throw std::logic_error("no exception neither value in result");
}

void ThrowResultHasNoException()
{
	throw std::logic_error("no exception in result - it keeps value");
}

std::exception_ptr VerifyResultException(std::exception_ptr&& ex)
{
	if (!ex)
	{
		try
		{
			ThrowInvalidResult();
		}
		catch (...)
		{
			return std::current_exception();
		}
	}
	return ex;
}
} // namespace detail
} // namespace isc
