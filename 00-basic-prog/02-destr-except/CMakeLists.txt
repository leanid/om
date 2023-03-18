cmake_minimum_required(VERSION 3.16)
project(destr_except CXX)
add_executable(destr_except main.cxx)
target_compile_features(destr_except PUBLIC cxx_std_20)

