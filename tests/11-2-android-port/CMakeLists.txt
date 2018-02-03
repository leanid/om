cmake_minimum_required(VERSION 3.6)

# Adds the CMakeLists.txt file located in the specified directory
# as a build dependency.
add_subdirectory( # Specifies the directory of the CMakeLists.txt file.
                  /home/leo/tools/SDL2-2.0.7

                  # Specifies the directory for the build outputs.
                  /home/leo/tools/SDL2-2.0.7_build )

# Adds the output of the additional CMake build as a prebuilt shared
# library and names it libSDL2.so.
#add_library( SDL2 SHARED IMPORTED )
#set_target_properties( SDL2 PROPERTIES IMPORTED_LOCATION
#                      /home/leo/tools/SDL2-2.0.7_build/${ANDROID_ABI}/libSDL2.so )

project("11-1-android-port" LANGUAGES CXX)

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
target_include_directories( engine PRIVATE /home/leo/tools/SDL2-2.0.7/include )
target_link_libraries( engine SDL2 SDL2main )
#target_compile_features(engine PRIVATE cxx_std_17)

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
    #target_link_libraries(engine
               #-lSDL2
               #-lGL
               #-lstdc++fs
    #           )
endif()

add_library(game SHARED game/game.cxx 
                        game/configuration_loader.hxx
                        game/game_object.hxx)
target_include_directories(game PRIVATE .)
#target_compile_features(game PRIVATE cxx_std_17)

target_link_libraries(game engine)
