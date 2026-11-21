cmake_minimum_required(VERSION 4.5.1)

project(11-shared-lock CXX)

add_executable(11-shared-lock "main.cxx")
target_compile_features(11-shared-lock PRIVATE cxx_std_20)





