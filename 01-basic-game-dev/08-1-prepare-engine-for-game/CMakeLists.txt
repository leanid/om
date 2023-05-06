cmake_minimum_required(VERSION 3.18)
project(08-1-prepare-engine-for-game)

add_library(engine-08-1 SHARED engine.cxx engine.hxx picopng.hxx)
target_compile_features(engine-08-1 PUBLIC cxx_std_20)

#set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(SDL3 REQUIRED)

if(WIN32)
    target_compile_definitions(engine-08-1
                               PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

target_link_libraries(engine-08-1 PRIVATE SDL3::SDL3-shared OpenGL::GL)

add_executable(game-08-1 game.cxx)
target_compile_features(game-08-1 PUBLIC cxx_std_20)

target_link_libraries(game-08-1 PRIVATE engine-08-1)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # using regular Clang or AppleClang
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()
