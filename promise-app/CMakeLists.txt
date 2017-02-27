
include(../cmake/use-msvc-static-runtime.cmake)

# Добавляем демонстрационное приложение
file(GLOB SRC_PROMISE_APP "*.cpp" "*.h")
add_executable(promise_app ${SRC_PROMISE_APP})

# Добавляем 'libpromise' в пути поиска заголовочных файлов.
target_include_directories(promise_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../libpromise)

if(UNIX)
target_link_libraries(promise_app libpromise boost_system pthread)
set_target_properties(promise_app PROPERTIES COMPILE_FLAGS "-std=c++14 -Wall -Wextra")
else(UNIX)
target_link_libraries(promise_app libpromise)
endif(UNIX)
