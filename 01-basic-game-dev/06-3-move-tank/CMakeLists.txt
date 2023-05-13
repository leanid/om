cmake_minimum_required(VERSION 3.16)
project(06-3-move-tank CXX)

add_library(engine-06-3 SHARED engine.cxx engine.hxx picopng.hxx)
target_compile_features(engine-06-3 PUBLIC cxx_std_20)

if(WIN32)
    target_compile_definitions(engine-06-3
                               PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(engine-06-3 PRIVATE SDL3::SDL3-shared OpenGL::GL)

add_executable(game-06-3 game.cxx)
target_compile_features(game-06-3 PUBLIC cxx_std_20)
target_link_libraries(game-06-3 PRIVATE engine-06-3)
