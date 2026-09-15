cmake_minimum_required(VERSION 3.22)

project(01-3-mem-layout CXX)

add_executable(01-3-mem-layout main.cxx)
target_compile_features(01-3-mem-layout PUBLIC cxx_std_20)

include(CTest)

add_test(NAME check_mem_layout_exit_zero COMMAND 01-3-mem-layout)
