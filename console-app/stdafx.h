#pragma once

// Если поставить 1, будем использовать boost::future вместо своих Promise
#define OPTION_USE_FUTURE_FACTORY 0

#if defined(_WIN32)
#include <SDKDDKVer.h>
#endif

#if OPTION_USE_FUTURE_FACTORY
#define BOOST_THREAD_PROVIDES_EXECUTORS
#define BOOST_THREAD_VERSION 4
#endif

#include <cstdlib>

#include <vector>
#include <chrono>
#include <memory>
#include <atomic>
#include <mutex>
