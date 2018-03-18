cmake_minimum_required(VERSION 3.9)
project(03-sdl-loop-to-engine-dll)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

add_library(engine-03-3 SHARED engine.cxx)
target_compile_features(engine-03-3 PUBLIC cxx_std_17)

if(WIN32)   
  target_compile_definitions(engine-03-3 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_library(SDL2_LIB NAMES SDL2)

if (MINGW)
    target_link_libraries(engine-03-3 
               -lmingw32 
               -lSDL2main 
               -lSDL2
               -mwindows
               )
elseif(UNIX)
    target_link_libraries(engine-03-3
               -lSDL2
               )
endif()

add_executable(game-03-3 game.cxx)
target_compile_features(game-03-3 PUBLIC cxx_std_17)

target_link_libraries(game-03-3 engine-03-3)
