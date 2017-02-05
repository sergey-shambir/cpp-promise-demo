project(cpp-demo)
cmake_minimum_required(VERSION 3.0)

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra")
endif(UNIX)


file(GLOB SSRC_LIBPROMISE "libpromise/*.cpp" "libpromise/*.h")
add_library(libpromise SRC_LIBPROMISE)

