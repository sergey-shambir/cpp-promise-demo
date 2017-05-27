
include(../cmake/use-msvc-static-runtime.cmake)
include(../cmake/enable-cxx17.cmake)

# Добавляем библиотеку libpromise
file(GLOB SRC_LIBPROMISE "*.cpp" "*.h" "promise/*.cpp" "promise/*.h" "platform/*.cpp" "platform/*.h")
add_library(libpromise ${SRC_LIBPROMISE})

# Включаем C++17
custom_enable_cxx17(libpromise)

# Добавляем пути поиска заголовочных файлов.
target_include_directories(libpromise PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
