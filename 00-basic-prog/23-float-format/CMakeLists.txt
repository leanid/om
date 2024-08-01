cmake_minimum_required(VERSION 3.30)

project(23-float-format LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

include(CheckIncludeFileCXX)
check_include_file_cxx("stdfloat" has_cxx_23_float_include)

include(CheckTypeSize)
check_type_size(
    std::float128_t
    float32
    LANGUAGE
    CXX)

if(has_cxx_23_float_include AND ${HAVE_float32})
    add_executable(23-float-format main.cxx)
    target_compile_features(23-float-format PRIVATE cxx_std_23)
else()
    message(STATUS "skip 23-float-format <stdfloat> not found")
endif(has_cxx_23_float_include AND ${HAVE_float32})
