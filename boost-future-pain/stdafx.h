#pragma once

// Boost пока ещё не готов к C++17, просим MSVC оставить std::unary_function.
#define _HAS_AUTO_PTR_ETC 1

#if defined(_WIN32)
#include <SDKDDKVer.h>
#endif

#define BOOST_THREAD_PROVIDES_EXECUTORS
#define BOOST_THREAD_VERSION 4
#include <boost/thread/future.hpp>

#include <cstdlib>

#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <vector>
