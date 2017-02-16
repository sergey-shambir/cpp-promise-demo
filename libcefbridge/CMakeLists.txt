project(libcefbridge)
cmake_minimum_required(VERSION 3.0)

# Добавляем библиотеку libcefbridge
file(GLOB SRC_CEFBRIDGE "*.cpp" "*.h")
add_library(libcefbridge ${SRC_CEFBRIDGE})

