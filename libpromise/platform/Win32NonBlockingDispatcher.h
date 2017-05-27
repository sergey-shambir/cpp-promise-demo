#pragma once
#include "IDispatcher.h"
#include <boost/noncopyable.hpp>
#include <memory>

#if defined(_WIN32)

namespace isc
{
class Win32NonBlockingDispatcher
	: public IDispatcher,
	  private boost::noncopyable
{
public:
	Win32NonBlockingDispatcher();
	~Win32NonBlockingDispatcher();

	void Dispatch(const Operation& operation) override;

private:
	struct Impl;

	std::unique_ptr<Impl> m_impl;
};
}

#endif
