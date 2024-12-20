cmake_minimum_required(VERSION 3.30)

project(34-doctest CXX)

add_executable(34-doctest main.cxx)
target_compile_features(34-doctest PRIVATE cxx_std_23)
