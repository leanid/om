cmake_minimum_required(VERSION 3.25)
project(08-2-engine-load-game-from-dll)

add_executable(engine-08-2 engine.cxx engine.hxx picopng.hxx)
set_target_properties(engine-08-2 PROPERTIES ENABLE_EXPORTS TRUE)
target_compile_features(engine-08-2 PUBLIC cxx_std_20)

find_package(OpenGL REQUIRED)
find_package(SDL3 REQUIRED)

if(WIN32)
    target_compile_definitions(engine-08-2
                               PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
    target_compile_definitions(
        engine-08-2 PRIVATE "-DOM_GAME=__declspec(dllimport)"
        )# for om_tat_sat import
endif(WIN32)

target_link_libraries(engine-08-2 PRIVATE SDL3::SDL3-shared OpenGL::GL)

add_library(game-08-2 MODULE game.cxx)
target_compile_features(game-08-2 PUBLIC cxx_std_20)

target_link_libraries(game-08-2 PRIVATE engine-08-2)

if(MSVC)
    # for all engine functions
    target_compile_definitions(game-08-2
                               PRIVATE "-DOM_DECLSPEC=__declspec(dllimport)")
    # for om_tat_sat export
    target_compile_definitions(game-08-2
                               PRIVATE "-DOM_GAME=__declspec(dllexport)")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # using regular Clang or AppleClang
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()
