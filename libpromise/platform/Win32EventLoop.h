#pragma once
#include "IEventLoop.h"
#include <boost/noncopyable.hpp>

#if defined(_WIN32)

namespace isprom
{
class Win32EventLoop
    : public IEventLoop
    , boost::noncopyable
{
public:
    Win32EventLoop();
    ~Win32EventLoop();

    void Post(const Operation &operation) override;
    void Run() override;
    void DeferQuit() override;

private:
    struct Impl;

    std::unique_ptr<Impl> m_impl;
};
}

#endif
