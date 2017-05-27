#pragma once
#include "../Types.h"

namespace isc
{
class IDispatcher;
using IDispatcherPtr = std::shared_ptr<IDispatcher>;

class IDispatcher
{
public:
	virtual ~IDispatcher() = default;

	virtual void Dispatch(const Operation& operation) = 0;
};
}
