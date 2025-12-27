# CMake 4.2 import std still experimental
cmake_minimum_required(VERSION 4.2 FATAL_ERROR)

# Set experimental flag to enable `import std` support from CMake.
# This must be enabled before C++ language support.
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD
  # This specific value changes as experimental support evolves. See
  # `Help/dev/experimental.rst` in the CMake source corresponding to
  # your CMake build for the exact value to use.
  "d0edc3af-4c50-42ea-a356-e2862fe7a444")

set(CMAKE_CXX_STANDARD 23) # minimal for import std; to work
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # stop using -std=gnu-23
set(CMAKE_CXX_FLAGS "-stdlib=libc++") # use llvm libc++
set(CMAKE_CXX_SCAN_FOR_MODULES ON) # automatic scan for import

# C++ needs to be enabled.
project(hello_import_std LANGUAGES CXX)

# Tell CMake that we explicitly want `import std`. This will initialize the
# property on all targets declared after this to 1
set(CMAKE_CXX_MODULE_STD 1)

# Make an executable.
add_executable(main hello.cxx)

