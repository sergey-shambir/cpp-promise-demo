
include(../cmake/use-msvc-static-runtime.cmake)
include(../cmake/enable-cxx17.cmake)

# Добавляем демонстрационное приложение
file(GLOB SRC_HTTPBIN_APP "*.cpp" "*.h")
add_executable(httpbin_app ${SRC_HTTPBIN_APP})

# Включаем C++17
custom_enable_cxx17(httpbin_app)

# Добавляем пути поиска заголовочных файлов.
target_include_directories(httpbin_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../libpromise ${CMAKE_CURRENT_SOURCE_DIR}/../libhttp)

# Добавляем библиотеки
target_link_libraries(httpbin_app libpromise libhttp curlcpp curl)
if(UNIX)
target_link_libraries(httpbin_app libpromise libhttp curlcpp curl boost_system pthread)
endif(UNIX)
