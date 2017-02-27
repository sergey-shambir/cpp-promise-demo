
include(../cmake/use-msvc-static-runtime.cmake)

# Добавляем демонстрационное приложение
file(GLOB SRC_BOOST_FUTURE_PAIN "*.cpp" "*.h")
add_executable(boost_future_pain ${SRC_BOOST_FUTURE_PAIN})

# Добавляем 'libpromise' в пути поиска заголовочных файлов.
target_include_directories(boost_future_pain PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../libpromise)

if(UNIX)
target_link_libraries(boost_future_pain libpromise boost_system pthread)
set_target_properties(boost_future_pain PROPERTIES COMPILE_FLAGS "-std=c++14 -Wall -Wextra")
else(UNIX)
target_link_libraries(boost_future_pain libpromise)
endif(UNIX)
