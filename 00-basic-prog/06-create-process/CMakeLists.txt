cmake_minimum_required(VERSION 3.16)

project(06-create-process CXX)
add_executable(06-create-process "main.cxx")
target_compile_features(06-create-process PRIVATE cxx_std_20)

add_executable(hello "hello.cxx")
target_compile_features(hello PRIVATE cxx_std_20)
