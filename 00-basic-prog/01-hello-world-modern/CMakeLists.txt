# CMake 4.5 supports `import std` natively
cmake_minimum_required(VERSION 4.5.1 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 23) # minimal for import std; to work
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # stop using -std=gnu-23
# set(CMAKE_CXX_FLAGS "-stdlib=libc++") # use llvm libc++
set(CMAKE_CXX_SCAN_FOR_MODULES ON) # automatic scan for import

# C++ needs to be enabled.
project(hello_import_std LANGUAGES CXX)

# Make an executable.
add_executable(main hello.cxx)
