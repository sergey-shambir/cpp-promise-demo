#pragma once
#include "../Types.h"

namespace isprom
{
class IDispatcher
{
public:
	virtual ~IDispatcher() = default;

	virtual void Post(const Operation &operation) = 0;
};
}
