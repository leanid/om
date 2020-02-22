cmake_minimum_required(VERSION 3.16.0)

project(glad C)

add_library(glad STATIC src/glad.c
                        include/glad/glad.h
                        include/KHR/khrplatform.h
                        )
target_include_directories(glad PUBLIC include)
