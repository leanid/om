cmake_minimum_required(VERSION 3.30)

project(23-float-format LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CheckIncludeFileCXX)
check_include_file_cxx("stdfloat" has_cxx_23_float_include)
message(STATUS "has_cxx_23_float_include: ${has_cxx_23_float_include}")

if (NOT has_cxx_23_float_include)
    message(STATUS "skip 23-float-format <stdfloat> not found")
    return()
endif()

include(CheckTypeSize)
set(CMAKE_EXTRA_INCLUDE_FILES "stdfloat")
check_type_size(std::float16_t float16 LANGUAGE CXX)
check_type_size(std::float32_t float32 LANGUAGE CXX)
check_type_size(std::float64_t float64 LANGUAGE CXX)
check_type_size(std::float128_t float128 LANGUAGE CXX)
unset(CMAKE_EXTRA_INCLUDE_FILES)
message(STATUS "HAVE_float16: ${HAVE_float16}")
message(STATUS "HAVE_float32: ${HAVE_float32}")
message(STATUS "HAVE_float64: ${HAVE_float64}")
message(STATUS "HAVE_float128: ${HAVE_float128}")

if(HAVE_float16 AND HAVE_float32 AND HAVE_float64 AND HAVE_float128)
    add_executable(23-float-format main.cxx)
    target_compile_features(23-float-format PRIVATE cxx_std_23)
else()
    message(STATUS "skip 23-float-format no std::float{16,32,64,128}_t support found")
endif()
