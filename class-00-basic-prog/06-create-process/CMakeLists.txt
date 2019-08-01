cmake_minimum_required(VERSION 3.14)

project(06-create-process)
add_executable(06-create-process "main.cxx")
target_compile_features(06-create-process PRIVATE cxx_std_17)

add_executable(hello "hello.cxx")
target_compile_features(hello PRIVATE cxx_std_17)
