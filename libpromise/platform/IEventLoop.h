#pragma once
#include "IDispatcher.h"
#include <chrono>

namespace isc
{
class IEventLoop : public IDispatcher
{
public:
	virtual ~IEventLoop() = default;

	virtual void Run() = 0;
	virtual void DeferQuit() = 0;
};
}
