#pragma once
#include "IEventLoop.h"
#include <boost/asio/io_service.hpp>
#include <boost/noncopyable.hpp>

namespace isprom
{
class AsioEventLoop
    : public IEventLoop
    , private boost::noncopyable
{
public:
    AsioEventLoop();
    ~AsioEventLoop();

    void Post(const Operation &operation) override;
    void Run() override;
    void DeferQuit() override;

private:
    boost::asio::io_service m_io;
    boost::asio::io_service::work m_work;
};
}
