
include(../cmake/use-msvc-static-runtime.cmake)
include(../cmake/enable-cxx17.cmake)
include(../cmake/use-vcpkg.cmake)

# Добавляем библиотеку libhttp
file(GLOB SRC_LIBHTTP "*.cpp" "*.h")
add_library(libhttp ${SRC_LIBHTTP})

# Включаем C++17
custom_enable_cxx17(libhttp)

# Добавляем пути поиска заголовочных файлов.
target_include_directories(libhttp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../libpromise ${VCPKG_INCLUDE_DIRS})
