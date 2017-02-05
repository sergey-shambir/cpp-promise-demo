project(cpp-demo)
cmake_minimum_required(VERSION 3.0)

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra")
endif(UNIX)


# Добавляем 'libpromise' в пути поиска заголовочных файлов.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libpromise)

# Добавляем библиотеку libpromise
file(GLOB SRC_LIBPROMISE "libpromise/*.cpp" "libpromise/*.h")
add_library(libpromise ${SRC_LIBPROMISE})

# Добавляем демонстрационное приложение
file(GLOB SRC_APP "app/*.cpp" "app/*.h")
add_executable(app ${SRC_APP})

if(UNIX)
    target_link_libraries(app libpromise boost_system pthread)
endif(UNIX)
