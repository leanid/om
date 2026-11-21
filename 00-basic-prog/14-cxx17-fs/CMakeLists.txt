cmake_minimum_required(VERSION 4.5.1)

project(14-cxx17-fs CXX)

add_executable(14-cxx17-fs main.cxx)
target_compile_features(14-cxx17-fs PRIVATE cxx_std_20)
