cmake_minimum_required(VERSION 3.9)
project("11-1-android-port")

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

add_executable(engine om/engine.cxx
                      om/backends/imgui_impl_sdl2.cpp
                      om/backends/imgui_impl_opengl3.cpp
                      om/imgui.cpp
                      om/imgui_draw.cpp
                      om/imgui_tables.cpp
                      om/imgui_widgets.cpp
                      om/imgui_demo.cpp
                      om/imgui.h
                      om/imgui_internal.h
                      om/imconfig.h
                      om/imstb_rectpack.h
                      om/imstb_textedit.h
                      om/imstb_truetype.h
                      om/gles20.hxx
                      om/engine.hxx
                      om/picopng.hxx)
set_target_properties(engine PROPERTIES ENABLE_EXPORTS TRUE)
target_compile_features(engine PRIVATE cxx_std_20)
target_include_directories(engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/om
                                          ${CMAKE_CURRENT_SOURCE_DIR}/om/backends)

if(WIN32)
  target_compile_definitions(engine PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
  target_compile_definitions(engine PRIVATE "-DIMGUI_API=__declspec(dllexport)")
endif(WIN32)

find_library(SDL2_LIB NAMES SDL2)

if (MINGW)
    target_link_libraries(engine
               -lmingw32
               -lSDL2main
               -lSDL2
               -mwindows
               -lopengl32
               #-lstdc++fs # no on linux mingw64 crosscompile failed
               )
elseif(UNIX)
    target_link_libraries(engine
               -lSDL2
               -lGL
               #-lstdc++fs
               )
elseif(MSVC)
    find_package(sdl2 REQUIRED)
    target_link_libraries(engine PRIVATE SDL2::SDL2 SDL2::SDL2main)
endif()

add_library(game SHARED game/game.cxx 
                        game/configuration_loader.hxx
                        game/game_object.hxx)
target_include_directories(game PRIVATE .)
target_compile_features(game PRIVATE cxx_std_20)

target_link_libraries(game engine)
