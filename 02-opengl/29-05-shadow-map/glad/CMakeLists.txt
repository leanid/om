cmake_minimum_required(VERSION 3.20...3.22)

project(glad_29_5 C)

add_library(
    glad_29_5 STATIC
    include/glad/glad.h
    include/KHR/khrplatform.h
    src/glad.c
)

target_include_directories(
    glad_29_5
    PUBLIC include
)
