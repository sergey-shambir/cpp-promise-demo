# На Windows добавьте переменную окружения VCPKG_SDK32,
#  указывающую на каталог "vcpkg/installed/ИМЯ_ТРИПЛЕТА"

set(VCPKG_INCLUDE_DIRS $ENV{VCPKG_SDK32}/include)
set(VCPKG_LIBRARY_DIRS $ENV{VCPKG_SDK32}/lib)
