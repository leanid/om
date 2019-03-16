cmake_minimum_required(VERSION 3.14)
project(06-5-sprite-editor)

add_library(engine-06-5 SHARED engine.cxx)
target_compile_features(engine-06-5 PUBLIC cxx_std_17)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/imgui-src)
target_link_libraries(engine-06-5 imgui_editor)

if(WIN32)
  target_compile_definitions(engine-06-5 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_library(SDL2_LIB NAMES SDL2)

if (MINGW)
    target_link_libraries(engine-06-5
               -lmingw32
               -lSDL2main
               -lSDL2
               -mwindows
               -lopengl32
               )
elseif(UNIX)
    target_link_libraries(engine-06-5
               -lSDL2
               -lGL
               )
elseif(MSVC)
    find_package(sdl2 REQUIRED)
    target_link_libraries(engine-06-5 PRIVATE SDL2::SDL2 SDL2::SDL2main
                          opengl32)
endif()

add_executable(06-5-sprite-editor sprite_editor.cxx sprite.cxx sprite.hxx)
target_compile_features(06-5-sprite-editor PUBLIC cxx_std_17)

target_link_libraries(06-5-sprite-editor engine-06-5)


if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

install(TARGETS engine-06-5 06-5-sprite-editor
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)

