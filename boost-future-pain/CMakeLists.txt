
include(../cmake/use-msvc-static-runtime.cmake)
include(../cmake/enable-cxx17.cmake)

# Добавляем демонстрационное приложение
file(GLOB SRC_BOOST_FUTURE_PAIN "*.cpp" "*.h")
add_executable(boost_future_pain ${SRC_BOOST_FUTURE_PAIN})

# Включаем C++17
custom_enable_cxx17(boost_future_pain)

# Добавляем 'libpromise' в пути поиска заголовочных файлов.
target_include_directories(boost_future_pain PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../libpromise)

# Добавляем библиотеки
target_link_libraries(boost_future_pain libpromise)
if(UNIX)
target_link_libraries(boost_future_pain boost_system libpromise pthread)
endif(UNIX)
