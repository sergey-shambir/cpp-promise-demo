
include(../cmake/use-msvc-static-runtime.cmake)

# Добавляем библиотеку libcefbridge
file(GLOB SRC_CEFBRIDGE "*.cpp" "*.h" "renderer/*.cpp" "renderer/*.h" "browser/*.cpp" "browser/*.h" "common/*.cpp" "common/*.h")
add_library(libcefbridge ${SRC_CEFBRIDGE})

# Добавляем путь к заголовкам cef
target_include_directories(libcefbridge PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../cef)
target_include_directories(libcefbridge PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty)

if(UNIX)
set_target_properties(libcefbridge PROPERTIES COMPILE_FLAGS "-std=c++14 -Wall -Wextra -Wno-unused")
endif(UNIX)
