
include(../cmake/use-msvc-static-runtime.cmake)
include(../cmake/enable-cxx17.cmake)

# Добавляем демонстрационное приложение
file(GLOB SRC_PROMISE_APP "*.cpp" "*.h")
add_executable(promise_app ${SRC_PROMISE_APP})

# Включаем C++17
custom_enable_cxx17(promise_app)

# Добавляем пути поиска заголовочных файлов.
target_include_directories(promise_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../libpromise)

# Добавляем библиотеки
target_link_libraries(promise_app libpromise libhttp)
if(UNIX)
target_link_libraries(promise_app lboost_system pthread)
endif(UNIX)
