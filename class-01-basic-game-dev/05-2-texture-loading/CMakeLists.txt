cmake_minimum_required(VERSION 3.9)
project(05-2-texture-loading)

add_library(engine-05-2 SHARED engine.cxx)
target_compile_features(engine-05-2 PUBLIC cxx_std_17)

if(WIN32)   
  target_compile_definitions(engine-05-2 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_library(SDL2_LIB NAMES SDL2)

if (MINGW)
    target_link_libraries(engine-05-2
               mingw32
               SDL2main 
               SDL2
               opengl32
               -mwindows
               )
elseif(UNIX)
    target_link_libraries(engine-05-2
               SDL2
               GL
               )
elseif(MSVC)
    find_package(sdl2 REQUIRED)
    target_link_libraries(engine-05-2 PRIVATE SDL2::SDL2 SDL2::SDL2main
                         opengl32)
endif()

add_executable(game-05-2 game.cxx)
target_compile_features(game-05-2 PUBLIC cxx_std_17)

target_link_libraries(game-05-2 engine-05-2)


if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

install(TARGETS engine-05-2 game-05-2
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)

