cmake_minimum_required(VERSION 3.9)
project(09-1-start-game-protorype)

add_executable(engine-09-1 om/engine.cxx om/engine.hxx om/picopng.hxx)
set_target_properties(engine-09-1 PROPERTIES ENABLE_EXPORTS TRUE)
target_compile_features(engine-09-1 PRIVATE cxx_std_17)

if(WIN32)
  target_compile_definitions(engine-09-1 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_library(SDL2_LIB NAMES SDL2)

if (MINGW)
    target_link_libraries(engine-09-1
               mingw32
               SDL2main
               SDL2
               -mwindows
               opengl32
               )
elseif(UNIX)
    target_link_libraries(engine-09-1
               SDL2
               GL
               )
elseif(MSVC)
    find_package(sdl2 REQUIRED)
    target_link_libraries(engine-09-1 PRIVATE SDL2::SDL2 SDL2::SDL2main)
endif()

add_library(game-09-1 SHARED game/game.cxx 
                        game/configuration_loader.hxx
                        game/game_object.hxx)
target_include_directories(game-09-1 PRIVATE .)
target_compile_features(game-09-1 PRIVATE cxx_std_17)

target_link_libraries(game-09-1 engine-09-1)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # using regular Clang or AppleClang
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()
