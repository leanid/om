cmake_minimum_required(VERSION 4.5.1)
project(03-bool-type CXX)
add_executable(03-bool-type main.cxx bool.hxx bool.cxx readme.md)
target_compile_features(03-bool-type PRIVATE cxx_std_20)

