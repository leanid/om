cmake_minimum_required(VERSION 3.9)
project(07-2-add-sound)

add_library(engine-07-2 SHARED engine.cxx engine.hxx picopng.hxx)
target_compile_features(engine-07-2 PUBLIC cxx_std_17)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

target_include_directories(engine-07-2 PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(engine-07-2 PRIVATE ${SDL2_LIBRARIES})

if(WIN32)
  target_compile_definitions(engine-07-2 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_library(SDL2_LIB NAMES SDL2)

if (MINGW)
    target_link_libraries(engine-07-2 PRIVATE
               SDL2::SDL2
               SDL2::SDL2main
               OpenGL::GL
               #-lmingw32
               #-lSDL2main
               #-lSDL2
               #-mwindows
               #-lopengl32
               )
elseif(APPLE)
    target_link_libraries(engine-07-2 PRIVATE
               -Wl,-framework,OpenGL
               )
elseif(UNIX)
    target_link_libraries(engine-07-2 PRIVATE
               SDL2::SDL2
               SDL2::SDL2main
               OpenGL::GL
               )
elseif(MSVC)
    target_link_libraries(engine-07-2 PRIVATE SDL2::SDL2 SDL2::SDL2main
                          OpenGL::GL)
endif()

add_executable(game-07-2 game.cxx)
target_compile_features(game-07-2 PUBLIC cxx_std_17)

target_link_libraries(game-07-2 PRIVATE engine-07-2)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_SCL_SECURE_NO_WARNINGS")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # using regular Clang or AppleClang
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

install(TARGETS engine-07-2 game-07-2
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)
