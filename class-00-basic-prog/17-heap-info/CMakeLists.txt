cmake_minimum_required(version 3.20...3.22)

project(17-heap-info CXX)

add_library(mem_info shared mem_info.hxx mem_info.cxx)
target_compile_features(mem_info private cxx_std_17)

add_executable(main main.cxx)
target_link_libraries(main private mem_info)
target_compile_features(main private cxx_std_17)
