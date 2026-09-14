cmake_minimum_required(VERSION 3.22)

# Пример Linux-only: mmap/munmap и интерпозиция символов malloc/new.
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "01-3-hello-mem: example is Linux-only, skipping")
    return()
endif()

project(01-3-hello-mem CXX)

add_executable(01-3-hello-mem main.cxx memory.cxx memory.hxx)
target_compile_features(01-3-hello-mem PUBLIC cxx_std_20)

include(CTest)

add_test(NAME check_hello_mem_exit_zero COMMAND 01-3-hello-mem)
