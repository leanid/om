cmake_minimum_required(VERSION 3.16)

project(09-concepts CXX)
add_executable(09-concepts "main.cxx")
target_compile_features(09-concepts PRIVATE cxx_std_20)
target_compile_options(09-concepts PRIVATE -fconcepts)


