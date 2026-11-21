cmake_minimum_required(VERSION 4.5.1)

project(glad_30_1 C)

add_library(
    glad_30_1 STATIC
    include/glad/glad.h
    include/KHR/khrplatform.h
    src/glad.c
)

target_include_directories(
    glad_30_1
    PUBLIC include
)
