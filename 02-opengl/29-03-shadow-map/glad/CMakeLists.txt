cmake_minimum_required(VERSION 3.20...3.22)

project(glad_29_3 C)

add_library(glad_29_3 STATIC src/glad.c
                        include/glad/glad.h
                        include/KHR/khrplatform.h
                        )
target_include_directories(glad_29_3 PUBLIC include)
