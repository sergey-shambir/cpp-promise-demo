#pragma once

#include "IDispatcher.h"
#include <boost/asio/io_service.hpp>
#include <boost/noncopyable.hpp>
#include <thread>
#include <vector>

namespace isc
{
class AsioThreadPool
	: public IDispatcher,
	  private boost::noncopyable
{
public:
	AsioThreadPool(unsigned threadsCount = std::thread::hardware_concurrency());
	~AsioThreadPool();

	void Dispatch(const Operation& operation) override;

private:
	boost::asio::io_service m_io;
	boost::asio::io_service::work m_work;
	std::vector<std::thread> m_threads;
};
}
