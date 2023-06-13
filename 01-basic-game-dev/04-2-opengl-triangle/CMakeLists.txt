cmake_minimum_required(VERSION 3.25)
project(04-2-opengl-triangle)

add_library(engine-04-2 SHARED engine.cxx engine.hxx)
target_compile_features(engine-04-2 PUBLIC cxx_std_20)

if(MINGW OR MSVC)
    # cannot find 01-basic-game-dev/03-4-sdl-loop-hot-reload/libengine-03-4.dll.a:
    # No such file or directory
    target_compile_definitions(engine-04-2
                               PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif()
# cmake-format: off
# target_compile_definitions(
#     engine-04-2
#     PRIVATE
#         $<$<CXX_COMPILER_ID:MSVC>:OM_DECLSPEC=__declspec(dllexport) >)
# cmake-format: on

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(engine-04-2 PRIVATE SDL3::SDL3-shared OpenGL::GL)

add_executable(game-04-2 game.cxx)
target_compile_features(game-04-2 PUBLIC cxx_std_20)
target_link_libraries(game-04-2 PRIVATE engine-04-2)
