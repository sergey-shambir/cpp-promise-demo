#include "stdafx.h"
#include "AsioEventLoop.h"

namespace isc
{

AsioEventLoop::AsioEventLoop()
	: m_work(m_io)
{
}

AsioEventLoop::~AsioEventLoop()
{
}

void AsioEventLoop::Dispatch(const Operation& operation)
{
	auto noexceptOperation = [operation] {
		try
		{
			operation();
		}
		catch (...)
		{
		}
	};
	m_io.post(std::move(noexceptOperation));
}

void AsioEventLoop::Run()
{
	m_io.run();
}

void AsioEventLoop::DeferQuit()
{
	Dispatch([this] {
		m_io.stop();
	});
}
}
