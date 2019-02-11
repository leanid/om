cmake_minimum_required(VERSION 3.12)

project(04-new-delete-overload CXX)

add_executable(04-new-delete-overload main.cxx)
target_compile_features(04-new-delete-overload PRIVATE cxx_std_17)
