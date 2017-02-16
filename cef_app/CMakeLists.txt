file(GLOB SRC_CEF_APP "*.cpp" "*.h")

include(../cmake/add_cef_based_executable.cmake)

add_cef_based_executable(cef_app "${SRC_CEF_APP}")

# Добавляем 'libpromise' в пути поиска заголовочных файлов.
target_include_directories(cef_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../libpromise)

# Добавляем 'lbcefbridge' в пути поиска заголовочных файлов.
target_include_directories(cef_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../libcefbridge)
