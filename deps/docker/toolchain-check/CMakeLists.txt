# Minimal project for the Docker image toolchain smoke test.
# Configured exactly like the OM `ninja-llvm` preset:
#   cmake -S . -B build -G "Ninja Multi-Config"
#       -DCMAKE_TOOLCHAIN_FILE=<repo>/cmake/toolchain/Linux.cmake
#       -DCMAKE_LINKER_TYPE=LLD -DCMAKE_CXX_SCAN_FOR_MODULES=ON
cmake_minimum_required(VERSION 4.5.1)

project(toolchain_check LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(toolchain_check main.cxx)
