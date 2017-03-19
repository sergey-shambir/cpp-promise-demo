#include "stdafx.h"
#include "AsioEventLoop.h"

namespace isprom
{

AsioEventLoop::AsioEventLoop()
    : m_work(m_io)
{
}

AsioEventLoop::~AsioEventLoop()
{
}

void AsioEventLoop::Post(const Operation &operation)
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
    Post([this] {
        m_io.stop();
    });
}
}
