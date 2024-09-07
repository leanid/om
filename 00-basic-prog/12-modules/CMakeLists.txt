cmake_minimum_required(VERSION 3.30)
# Set experimental flag to enable `import std` support from CMake.
# This must be enabled before C++ language support.
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD
    # This specific value changes as experimental support evolves. See
    # `Help/dev/experimental.rst` in the CMake source corresponding to
    # your CMake build for the exact value to use.
    "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
# C++ needs to be enabled.
project("modules-12" LANGUAGES CXX)

set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
# Currently CMake requires extensions enabled when using import std.
# https://gitlab.kitware.com/cmake/cmake/-/issues/25916
# https://gitlab.kitware.com/cmake/cmake/-/issues/25539
set(CMAKE_CXX_EXTENSIONS ON)
# implement it when cmake+ninja+g++ support modules out of the box

message(STATUS "c++ compiler: ${CMAKE_CXX_COMPILER_ID}-"
               "${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "is import std;"
               " support present: ${CMAKE_CXX_COMPILER_IMPORT_STD}")

add_executable(modules-12 hello.cxx main.cxx)

# how to build it on Linux with clang++?
# mkdir build
# cd build
# cmake -DCMAKE_CXX_FLAGS="-stdlib=libc++" -G Ninja ..
