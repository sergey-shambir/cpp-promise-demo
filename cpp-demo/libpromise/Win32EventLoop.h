#pragma once
#include "IEventLoop.h"
#include <boost/noncopyable.hpp>

namespace isprom
{
class Win32EventLoop
	: public IEventLoop
	, boost::noncopyable
{
public:
	Win32EventLoop();
	~Win32EventLoop();

	void Run() override;
	void Post(const Operation &operation) override;

private:
	struct Impl;

	std::unique_ptr<Impl> m_impl;
};
}
