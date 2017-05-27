#pragma once
#include "IEventLoop.h"

#if defined(_WIN32)

namespace isc
{
class Win32EventLoop
	: public IEventLoop
{
public:
	Win32EventLoop();
	~Win32EventLoop();

	Win32EventLoop(const Win32EventLoop&) = delete;
	Win32EventLoop& operator=(const Win32EventLoop&) = delete;

	void Dispatch(const Operation& operation) override;
	void Run() override;
	void DeferQuit() override;

private:
	struct Impl;

	std::unique_ptr<Impl> m_impl;
};
}

#endif
