cmake_minimum_required(VERSION 3.9)
project(07-2-add-sound)

add_library(engine-07-2 SHARED engine.cxx engine.hxx picopng.hxx)
target_compile_features(engine-07-2 PUBLIC cxx_std_17)

if(WIN32)
  target_compile_definitions(engine-07-2 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_library(SDL2_LIB NAMES SDL2)

if (MINGW)
    target_link_libraries(engine-07-2
               -lmingw32
               -lSDL2main
               -lSDL2
               -mwindows
               -lopengl32
               )
elseif(UNIX)
    target_link_libraries(engine-07-2
               -lSDL2
               -lGL
               )
elseif(MSVC)
    find_package(sdl2 REQUIRED)
    target_link_libraries(engine-07-2 PRIVATE SDL2::SDL2 SDL2::SDL2main
                          opengl32)
endif()

add_executable(game-07-2 game.cxx)
target_compile_features(game-07-2 PUBLIC cxx_std_17)

target_link_libraries(game-07-2 engine-07-2)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /std:c++17")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_SCL_SECURE_NO_WARNINGS")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # using regular Clang or AppleClang
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()
