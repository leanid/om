cmake_minimum_required(VERSION 3.16.0)

project(glad_20_3 C)

add_library(glad_20_3 STATIC src/glad.c
                        include/glad/glad.h
                        include/KHR/khrplatform.h
                        )
target_include_directories(glad_20_3 PUBLIC include)
