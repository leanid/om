cmake_minimum_required(VERSION 3.16)
project(07-1-add-sound CXX)

add_executable(sound_test-07-1 sound_test.cxx)
target_compile_features(sound_test-07-1 PUBLIC cxx_std_20)

find_package(SDL3 REQUIRED)

target_link_libraries(sound_test-07-1 PRIVATE SDL3::SDL3-shared)
