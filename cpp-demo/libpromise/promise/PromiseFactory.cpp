#include "stdafx.h"
#include "PromiseFactory.h"

namespace isprom
{

PromiseFactory::PromiseFactory(isprom::IDispatcher &callDispatcher, isprom::IDispatcher &callbackDispatcher)
    : m_callDispatcher(callDispatcher)
    , m_callbackDispatcher(callbackDispatcher)
{
}

}
