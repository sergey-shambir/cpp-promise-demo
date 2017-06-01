cmake_minimum_required(VERSION 3.7)

# Создавать только две конфигурации: Debug и Release.
set(CMAKE_CONFIGURATION_TYPES Debug Release)

project(cpp-promise-demo)

# Заставим MSVC использовать static версию C++ runtime вместо DLL
include(cmake/use-msvc-static-runtime.cmake)

# --- Свойства для сборки демонстрационных приложений ---
add_subdirectory(libpromise)
add_subdirectory(libhttp)
add_subdirectory(promise-app)
add_subdirectory(boost-future-pain)
