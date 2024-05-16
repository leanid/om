cmake_minimum_required(VERSION 3.26)

project(23-float-format LANGUAGES CXX)

include(CheckIncludeFileCXX)
check_include_file_cxx("stdfloat" has_cxx_23_float_include)

if(has_cxx_23_float_include)
    add_executable(23-float-format main.cxx)
    target_compile_features(23-float-format PRIVATE cxx_std_23)
else()
    message(STATUS "skip 23-float-format <stdfloat> not found")
endif(has_cxx_23_float_include)
