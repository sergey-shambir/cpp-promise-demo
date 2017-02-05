#pragma once
#include "IDispatcher.h"
#include <memory>
#include <boost/noncopyable.hpp>


#if defined(_WIN32)

namespace isprom
{
class Win32NonBlockingDispatcher
	: public IDispatcher
	, private boost::noncopyable
{
public:
	Win32NonBlockingDispatcher();
	~Win32NonBlockingDispatcher();

	void Post(const Operation &operation) override;

	// TODO: timer-based scheduler can be useful,
	//  but is out of scope for my project right now.
#if 0
	class ITimer;
	using ITimerPtr = std::unique_ptr<ITimer>;
	virtual ITimerPtr ScheduleOnce(const Operation &operation, std::chrono::milliseconds const& timeout) = 0;
	virtual ITimerPtr ScheduleRepeated(const Operation &operation, std::chrono::milliseconds const& timeout) = 0;
#endif

private:
	struct Impl;

	std::unique_ptr<Impl> m_impl;
};
}

#endif
