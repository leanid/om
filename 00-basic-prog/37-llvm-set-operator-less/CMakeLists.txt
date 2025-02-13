cmake_minimum_required(VERSION 3.31)

project(llvm-set-operator-less CXX)

add_executable(llvm-set-operator-less main.cxx)

target_compile_features(llvm-set-operator-less PRIVATE cxx_std_20)
