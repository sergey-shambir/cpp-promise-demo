#include "stdafx.h"
#include "AsioThreadPool.h"
#include <algorithm>

namespace isprom
{
AsioThreadPool::AsioThreadPool(unsigned threadsCount)
	: m_io(std::max(1u, threadsCount))
	, m_work(m_io)
{
	threadsCount = std::max(1u, threadsCount);
	for (unsigned ti = 0; ti < threadsCount; ++ti)
	{
		m_threads.emplace_back([this] {
			m_io.run();
		});
	}
}

AsioThreadPool::~AsioThreadPool()
{
	m_io.stop();
	for (auto &thread : m_threads)
	{
		thread.join();
	}
}

void AsioThreadPool::Post(const Operation &operation)
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
}
