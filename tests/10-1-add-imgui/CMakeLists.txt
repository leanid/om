cmake_minimum_required(VERSION 3.9)
project(10-1-add-imgui)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

add_executable(engine-10-1 om/engine.cxx
                      om/imgui_impl_sdl_gl3.cpp
                      om/imgui.cpp
                      om/imgui_draw.cpp
                      om/imgui_demo.cpp
                      om/imgui.h
                      om/imgui_impl_sdl_gl3.h
                      om/imgui_internal.h
                      om/imconfig.h
                      om/gles20.hxx
                      om/engine.hxx 
                      om/picopng.hxx)
set_target_properties(engine-10-1 PROPERTIES ENABLE_EXPORTS TRUE)
target_compile_features(engine-10-1 PRIVATE cxx_std_17)

if(WIN32)
  target_compile_definitions(engine-10-1 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
  target_compile_definitions(engine-10-1 PRIVATE "-DIMGUI_API=__declspec(dllexport)")
endif(WIN32)

find_library(SDL2_LIB NAMES SDL2)

if (MINGW)
    target_link_libraries(engine-10-1
               mingw32
               SDL2main
               SDL2
               -mwindows
               opengl32
               )
elseif(UNIX)
    target_link_libraries(engine-10-1
               SDL2
               GL
               )
endif()

add_library(game-10-1 SHARED game/game.cxx 
                        game/configuration_loader.hxx
                        game/game_object.hxx)
target_include_directories(game-10-1 PRIVATE .)
target_compile_features(game-10-1 PRIVATE cxx_std_17)

target_link_libraries(game-10-1 engine-10-1)
