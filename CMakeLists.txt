cmake_minimum_required(VERSION 3.2)

# Создавать только две конфигурации: Debug и Release.
set(CMAKE_CONFIGURATION_TYPES Debug Release)

project(cpp-promise-demo)

include(cmake/use-msvc-static-runtime.cmake)

# Заставим MSVC использовать static версию C++ runtime вместо DLL
# include(cmake/use-msvc-static-runtime.cmake)

# --- Свойства для сборки с CEF ---

# Use folders in the resulting project files.
set_property(GLOBAL PROPERTY OS_FOLDERS ON)

# Устанавливаем CEF_ROOT на подмодуль cef
set(CEF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/cef")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")
# Выполняем FindCEF.cmake, который теперь попадает в CMAKE_MODULE_PATH.
find_package(CEF REQUIRED)

# Добавляем в список путей модулей CMake путь к ./cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#
# Define CEF-based targets.
#

# Include the libcef_dll_wrapper target.
# Comes from the libcef_dll/CMakeLists.txt file in the binary distribution
# directory.
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)


# --- Свойства для сборки демонстрационных приложений ---

# Include application targets.
# Comes from the <target>/CMakeLists.txt file in the current directory.
add_subdirectory(libpromise)
add_subdirectory(libcefbridge)
add_subdirectory(console-app)
add_subdirectory(cef_app)


