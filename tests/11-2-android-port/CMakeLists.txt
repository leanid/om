cmake_minimum_required(VERSION 3.6)

if(SDL2_SRC_DIR)
add_subdirectory( # Specifies the directory of the CMakeLists.txt file.
                  ${SDL2_SRC_DIR}
                  # Specifies the directory for the build outputs.
                  ${SDL2_BUILD_DIR} )  
endif()

project("11-2-android-port" LANGUAGES C CXX)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall -Wextra -pedantic -Werror")
endif()

add_library(engine SHARED om/engine.cxx
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
set_target_properties(engine PROPERTIES ENABLE_EXPORTS TRUE)

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
    if(SDL2_SRC_DIR)
        target_include_directories( engine PRIVATE ${SDL2_SRC_DIR}/include )
        target_link_libraries( engine SDL2 SDL2main )
    else()
        target_link_libraries(engine
               -lSDL2
               -lGL
               #-lstdc++fs
               )
    endif()
endif()

add_library(game SHARED game/game.cxx 
                        game/configuration_loader.hxx
                        game/game_object.hxx)
target_include_directories(game PRIVATE .)

target_link_libraries(game engine)
