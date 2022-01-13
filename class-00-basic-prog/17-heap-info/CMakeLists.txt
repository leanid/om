cmake_minimum_required(VERSION 3.20...3.22)

project(17-heap-info CXX)

add_library(mem_info SHARED
                     mem_info.hxx
                     mem_info.cxx)

add_executable(main main.cxx)

target_link_libraries(main PRIVATE mem_info)

