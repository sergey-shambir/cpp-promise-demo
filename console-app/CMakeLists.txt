project(console-app)
cmake_minimum_required(VERSION 3.0)

# Добавляем 'libpromise' в пути поиска заголовочных файлов.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libpromise)

# Добавляем демонстрационное приложение
file(GLOB SRC_CONSOLE_APP "*.cpp" "*.h")
add_executable(console_app ${SRC_CONSOLE_APP})

# set_target_properties(console_app PROPERTIES COMPILE_FLAGS "-std=c++14 -Wall -Wextra")

target_link_libraries(console_app libpromise boost_system pthread)
