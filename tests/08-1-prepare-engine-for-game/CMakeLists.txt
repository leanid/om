cmake_minimum_required(VERSION 3.9)
project(08-1-prepare-engine-for-game)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

add_library(engine-08-1 SHARED engine.cxx engine.hxx picopng.hxx)
target_compile_features(engine-08-1 PUBLIC cxx_std_17)

if(WIN32)
  target_compile_definitions(engine-08-1 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_library(SDL2_LIB NAMES SDL2)

if (MINGW)
    target_link_libraries(engine-08-1
               -lmingw32
               -lSDL2main
               -lSDL2
               -mwindows
               -lopengl32
               )
elseif(UNIX)
    target_link_libraries(engine-08-1
               -lSDL2
               -lGL
               )
endif()

add_executable(game-08-1 game.cxx)
target_compile_features(game-08-1 PUBLIC cxx_std_17)

target_link_libraries(game-08-1 engine-08-1)
