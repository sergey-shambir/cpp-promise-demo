#include "stdafx.h"
#include "FutureFactory.h"

#if OPTION_PAIN_LEVEL == 0 || OPTION_PAIN_LEVEL == 4

namespace isprom
{

FutureFactory::FutureFactory(IDispatcher &callDispatcher, IDispatcher &callbackDispatcher)
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

}

#endif
