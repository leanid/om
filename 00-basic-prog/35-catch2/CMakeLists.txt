cmake_minimum_required(VERSION 3.30)

project(35-catch2 CXX)

add_library(amalgamated_catch2 STATIC catch_amalgamated.cpp
                                      catch_amalgamated.hpp)
target_compile_features(amalgamated_catch2 PRIVATE cxx_std_23)

add_executable(35-catch2 main.cxx)
target_link_libraries(35-catch2 PRIVATE amalgamated_catch2)
target_compile_features(35-catch2 PRIVATE cxx_std_23)

enable_testing()

add_test(NAME 35-catch2 COMMAND 35-catch2)
