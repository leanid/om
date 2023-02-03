cmake_minimum_required(VERSION 3.16)

project(05-signal-handler CXX)
add_executable(05-signal-handler "main.cpp")
target_compile_features(05-signal-handler PRIVATE cxx_std_20)

add_executable(05-signal-handler-throw "throw_from_sig_handler.cpp")
target_compile_features(05-signal-handler-throw PRIVATE cxx_std_20)
