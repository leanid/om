cmake_minimum_required(VERSION 3.9)
project(07-add-sound)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

find_library(SDL2_LIB NAMES SDL2)

add_executable(sound_test audio.c test.c audio.h)

if (MINGW)
    target_link_libraries(sound_test
               -lmingw32
               -lSDL2main
               -lSDL2
               -mwindows
               -lopengl32
               )
elseif(UNIX)
    target_link_libraries(sound_test
               -lSDL2
               -lGL
               )
endif()
