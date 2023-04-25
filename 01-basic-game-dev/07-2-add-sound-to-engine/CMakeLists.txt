cmake_minimum_required(VERSION 3.16)
project(07-2-add-sound)

add_library(engine-07-2 SHARED engine.cxx engine.hxx picopng.hxx)
target_compile_features(engine-07-2 PUBLIC cxx_std_20)

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

if(WIN32)
    target_compile_definitions(engine-07-2
                               PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

target_link_libraries(engine-07-2 PRIVATE SDL3::SDL3-shared OpenGL::GL)

add_executable(game-07-2 game.cxx)
target_compile_features(game-07-2 PUBLIC cxx_std_20)

target_link_libraries(game-07-2 PRIVATE engine-07-2)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_SCL_SECURE_NO_WARNINGS")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # using regular Clang or AppleClang
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()
