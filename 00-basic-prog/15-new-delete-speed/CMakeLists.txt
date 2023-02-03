cmake_minimum_required(VERSION 3.16)

project(15-new-delete CXX)

add_executable(15-new-delete main.cxx)
target_compile_features(15-new-delete PRIVATE cxx_std_20)
