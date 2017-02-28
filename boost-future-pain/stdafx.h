#pragma once

#if defined(_WIN32)
#include <SDKDDKVer.h>
#endif

#define BOOST_THREAD_PROVIDES_EXECUTORS
#define BOOST_THREAD_VERSION 4
#include <boost/thread/future.hpp>

#include <cstdlib>

#include <vector>
#include <chrono>
#include <memory>
#include <atomic>
#include <mutex>
