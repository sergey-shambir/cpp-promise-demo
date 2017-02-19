
include(../cmake/use-msvc-static-runtime.cmake)

# Добавляем демонстрационное приложение
file(GLOB SRC_CONSOLE_APP "*.cpp" "*.h")
add_executable(console_app ${SRC_CONSOLE_APP})

# Добавляем 'libpromise' в пути поиска заголовочных файлов.
target_include_directories(console_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../libpromise)

if(UNIX)
target_link_libraries(console_app libpromise boost_system pthread)
set_target_properties(console_app PROPERTIES COMPILE_FLAGS "-std=c++14 -Wall -Wextra")
else(UNIX)
target_link_libraries(console_app libpromise)
endif(UNIX)
