cmake_minimum_required(VERSION 3.16)
project(05-1-vertex-morphing)

add_library(engine-05-1 SHARED engine.cxx engine.hxx)
target_compile_features(engine-05-1 PUBLIC cxx_std_20)

if(WIN32)
    target_compile_definitions(engine-05-1
                               PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(engine-05-1 PRIVATE SDL3::SDL3-shared OpenGL::GL)
add_executable(game-05-1 game.cxx)
target_compile_features(game-05-1 PUBLIC cxx_std_20)
target_link_libraries(game-05-1 PRIVATE engine-05-1)
