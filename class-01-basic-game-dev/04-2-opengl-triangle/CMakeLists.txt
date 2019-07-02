cmake_minimum_required(VERSION 3.9)
project(04-2-opengl-triangle)

add_library(engine-04-2 SHARED engine.cxx)
target_compile_features(engine-04-2 PUBLIC cxx_std_17)

if(WIN32)   
  target_compile_definitions(engine-04-2 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_package(sdl2 REQUIRED)
target_include_directories(engine-04-2 PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(engine-04-2 PRIVATE ${SDL2_LIBRARIES})

if (MINGW)
    target_link_libraries(engine-04-2 
               -lmingw32 
               -lSDL2main 
               -lSDL2
               -mwindows
               -lopengl32
               )
elseif(APPLE)
    target_link_libraries(engine-04-2 PRIVATE
               -Wl,-framework,OpenGL
               )
elseif(UNIX)
    target_link_libraries(engine-04-2 PRIVATE
               -lSDL2
               -lGL
               )
elseif(MSVC)
    target_link_libraries(engine-04-2 PRIVATE SDL2::SDL2 SDL2::SDL2main
                         opengl32)
endif()

add_executable(game-04-2 game.cxx)
target_compile_features(game-04-2 PUBLIC cxx_std_17)

target_link_libraries(game-04-2 engine-04-2)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

install(TARGETS engine-04-2 game-04-2
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)

