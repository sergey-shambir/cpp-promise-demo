#include "stdafx.h"
#include "FutureFactory.h"

FutureFactory::FutureFactory(isprom::IDispatcher &callDispatcher, isprom::IDispatcher &callbackDispatcher)
    : m_callDispatcher(callDispatcher)
    , m_callbackDispatcher(callbackDispatcher)
{
    m_closed = false;
}

void FutureFactory::close()
{
    m_closed = true;
}

bool FutureFactory::closed()
{
    return m_closed;
}

void FutureFactory::submit(work && closure)
{
    m_callbackDispatcher.Post(std::move(closure));
}

bool FutureFactory::try_executing_one()
{
    return false;
}
