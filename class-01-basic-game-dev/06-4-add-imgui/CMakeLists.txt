cmake_minimum_required(VERSION 3.9)
project(06-4-add-imgui)

add_library(engine-06-4 SHARED engine.cxx)
target_compile_features(engine-06-4 PUBLIC cxx_std_17)

if(WIN32)   
  target_compile_definitions(engine-06-4 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_library(SDL2_LIB NAMES SDL2)

if (MINGW)
    target_link_libraries(engine-06-4
               -lmingw32 
               -lSDL2main 
               -lSDL2
               -mwindows
               -lopengl32
               )
elseif(UNIX)
    target_link_libraries(engine-06-4
               -lSDL2
               -lGL
               )
elseif(MSVC)
    find_package(sdl2 REQUIRED)
    target_link_libraries(engine-06-4 PRIVATE SDL2::SDL2 SDL2::SDL2main
                          opengl32)
endif()

add_executable(game-06-4 game.cxx)
target_compile_features(game-06-4 PUBLIC cxx_std_17)

target_link_libraries(game-06-4 engine-06-4)


if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

install(TARGETS engine-06-4 game-06-4
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)

