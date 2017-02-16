# Добавляем библиотеку libcefbridge
file(GLOB SRC_CEFBRIDGE "*.cpp" "*.h")
add_library(libcefbridge ${SRC_CEFBRIDGE})
set_target_properties(libcefbridge PROPERTIES COMPILE_FLAGS "-std=c++14 -Wall -Wextra")

# Добавляем путь к заголовкам cef
target_include_directories(libcefbridge PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../cef)
