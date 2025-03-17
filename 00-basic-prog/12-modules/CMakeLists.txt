# this example currently works on msvc-2022 x64 with Ninja generator
cmake_minimum_required(VERSION 4.0)
# Set experimental flag to enable `import std` support from CMake.
# This must be enabled before C++ language support.
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD
    # This specific value changes as experimental support evolves. See
    # `Help/dev/experimental.rst` in the CMake source corresponding to
    # your CMake build for the exact value to use.
    # https://github.com/Kitware/CMake/blob/master/Help/dev/experimental.rst
    "a9e1cf81-9932-4810-974b-6eccaf14e457")
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
message(STATUS "toolchain [import std;]" " support present for standards: "
               "[${CMAKE_CXX_COMPILER_IMPORT_STD}]")
message(STATUS "generator is: ${CMAKE_GENERATOR}")

if(23 IN_LIST CMAKE_CXX_COMPILER_IMPORT_STD)
    message(STATUS "current toolchain support [import std;]")
else()
    message(STATUS "current toolchain DOES NOT support [import std;]"
                   "skip modules-12 example")
    return()
endif()

if(CMAKE_GENERATOR STREQUAL "Ninja" OR CMAKE_GENERATOR STREQUAL
                                       "Ninja Multi-Config")
    add_executable(modules-12 "main.cxx")
    target_sources(
        modules-12
        PRIVATE FILE_SET
                modules
                TYPE
                CXX_MODULES
                FILES
                "hello.cxx")
    message(STATUS "use import std; with current toolchain")
else()
    message(STATUS "Only [Ninja] or [Ninja Multi-Config]"
                   " generator supported in cmake 3.30")
endif()

# how to build it on Linux with clang++?
# mkdir build
# cd build
# cmake -DCMAKE_CXX_FLAGS="-stdlib=libc++" -G Ninja ..
