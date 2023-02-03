cmake_minimum_required(VERSION 3.20...3.22)

project(17-heap-info CXX)

add_library(mem_info SHARED mem_info.cxx mem_info.hxx)
target_compile_features(mem_info PRIVATE cxx_std_20)

add_executable(main main.cxx)
target_link_libraries(main PRIVATE mem_info)
target_compile_features(main PRIVATE cxx_std_20)
