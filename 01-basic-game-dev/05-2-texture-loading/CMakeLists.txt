cmake_minimum_required(VERSION 3.16)
project(05-2-texture-loading)

add_library(engine-05-2 SHARED engine.cxx
                               engine.hxx
                               picopng.hxx)
target_compile_features(engine-05-2 PUBLIC cxx_std_20)

if(WIN32)
  target_compile_definitions(engine-05-2 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

target_include_directories(engine-05-2 PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(engine-05-2 PRIVATE ${SDL2_LIBRARIES})

if (MINGW)
    target_link_libraries(engine-05-2 PRIVATE
               SDL2::SDL2
               SDL2::SDL2main
               OpenGL::GL
               #mingw32
               #SDL2main
               #SDL2
               #opengl32
               #-mwindows
               )
elseif(APPLE)
    target_link_libraries(engine-05-2 PRIVATE
            -Wl,-framework,OpenGL
            )
elseif(UNIX)
    target_link_libraries(engine-05-2 PRIVATE
               SDL2::SDL2
               SDL2::SDL2main
               OpenGL::GL
               )
elseif(MSVC)
    target_link_libraries(engine-05-2 PRIVATE SDL2::SDL2 SDL2::SDL2main
                         OpenGL::GL)
endif()

add_executable(game-05-2 game.cxx)
target_compile_features(game-05-2 PUBLIC cxx_std_20)

target_link_libraries(game-05-2 PRIVATE engine-05-2)


if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

install(TARGETS engine-05-2 game-05-2
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)

