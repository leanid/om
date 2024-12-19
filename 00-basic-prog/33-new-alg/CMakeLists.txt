cmake_minimum_required(VERSION 3.30)

project(33-new-alg CXX)

add_executable(33-new-alg main.cxx)
target_compile_features(33-new-alg PRIVATE cxx_std_23)
