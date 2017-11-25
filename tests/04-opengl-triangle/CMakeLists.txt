cmake_minimum_required(VERSION 3.9)
project(03-sdl-loop-to-engine-dll)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

add_library(engine SHARED engine.cxx)
target_compile_features(engine PUBLIC cxx_std_17)

if(WIN32)   
  target_compile_definitions(engine PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_library(SDL2_LIB NAMES SDL2)

if (MINGW)
    target_link_libraries(engine 
               -lmingw32 
               -lSDL2main 
               -lSDL2
               -mwindows
               -lopengl32
               )
elseif(UNIX)
    target_link_libraries(engine
               -lSDL2
               -lGL
               )
endif()

add_executable(game game.cxx)
target_compile_features(game PUBLIC cxx_std_17)

target_link_libraries(game engine)
