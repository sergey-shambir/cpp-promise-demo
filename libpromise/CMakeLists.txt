# Добавляем библиотеку libpromise
file(GLOB SRC_LIBPROMISE "*.cpp" "*.h" "promise/*.cpp" "promise/*.h" "platform/*.cpp" "platform/*.h")
add_library(libpromise ${SRC_LIBPROMISE})
target_include_directories(libpromise PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(libpromise PROPERTIES COMPILE_FLAGS "-std=c++14 -Wall -Wextra")
