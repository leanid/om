cmake_minimum_required(VERSION 3.26)
project(04-1-opengl-minimal CXX C)

add_library(
    engine-04-1 SHARED
    engine.cxx
    engine.hxx
    glad/glad.c
    glad/glad.h
    KHR/khrplatform.h)
target_compile_features(engine-04-1 PUBLIC cxx_std_20)
target_include_directories(engine-04-1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    target_compile_definitions(engine-04-1
                               PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(engine-04-1 PRIVATE SDL3::SDL3-shared)

add_executable(game-04-1 game.cxx)
target_compile_features(game-04-1 PUBLIC cxx_std_20)
target_link_libraries(game-04-1 PRIVATE engine-04-1)
