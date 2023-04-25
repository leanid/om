cmake_minimum_required(VERSION 3.26)
project(06-4-add-imgui)

add_library(engine-06-4 SHARED engine.cxx engine.hxx picopng.hxx)
target_compile_features(engine-06-4 PUBLIC cxx_std_20)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/imgui-src)
target_link_libraries(engine-06-4 PRIVATE imgui)

if(WIN32)
    target_compile_definitions(engine-06-4
                               PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(engine-06-4 PRIVATE SDL3::SDL3-shared OpenGL::GL)

add_executable(game-06-4 game.cxx)
target_compile_features(game-06-4 PUBLIC cxx_std_20)
target_link_libraries(game-06-4 PRIVATE engine-06-4)
