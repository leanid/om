cmake_minimum_required(VERSION 3.16.0)

project(glad_21_4 C)

add_library(glad_21_4 STATIC src/glad.c
                        include/glad/glad.h
                        include/KHR/khrplatform.h
                        )
target_include_directories(glad_21_4 PUBLIC include)
