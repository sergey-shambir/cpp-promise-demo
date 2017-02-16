project(cpp-demo)
cmake_minimum_required(VERSION 3.0)

# Добавляем библиотеку libpromise
file(GLOB SRC_LIBPROMISE "*.cpp" "*.h" "promise/*.cpp" "promise/*.h" "platform/*.cpp" "platform/*.h")
add_library(libpromise ${SRC_LIBPROMISE})

