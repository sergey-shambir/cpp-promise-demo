#pragma once

#include "IDispatcher.h"
#include <boost/noncopyable.hpp>
#include <boost/asio/io_service.hpp>
#include <thread>
#include <vector>

namespace isprom
{
class AsioThreadPool
	: public IDispatcher
	, private boost::noncopyable
{
public:
	AsioThreadPool(unsigned threadsCount = std::thread::hardware_concurrency());
	~AsioThreadPool();

	void Post(const Operation &operation) override;

private:
	boost::asio::io_service m_io;
	boost::asio::io_service::work m_work;
	std::vector<std::thread> m_threads;
};
}
