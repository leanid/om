cmake_minimum_required(VERSION 4.5.1)

project(10-debug-macro CXX)

add_executable(10-debug-macro "main.cxx")
target_compile_features(10-debug-macro PRIVATE cxx_std_20)





