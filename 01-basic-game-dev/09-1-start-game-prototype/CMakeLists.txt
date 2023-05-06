cmake_minimum_required(VERSION 3.25)
project(09-1-start-game-protorype)

add_executable(
    engine-09-1
    om/engine.cxx
    om/engine.hxx
    om/math.hxx
    om/picopng.hxx)
set_target_properties(engine-09-1 PROPERTIES ENABLE_EXPORTS TRUE)
target_compile_features(engine-09-1 PRIVATE cxx_std_20)

if(WIN32)
    target_compile_definitions(engine-09-1
                               PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)
target_link_libraries(engine-09-1 PRIVATE SDL3::SDL3-shared OpenGL::GL)

add_library(
    game-09-1 SHARED
    game/configuration_loader.hxx
    game/game.cxx
    game/game_object.hxx
    res/background.png
    res/identity_quad.txt
    res/level_01.txt
    res/tank.png)
target_include_directories(game-09-1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(game-09-1 PRIVATE cxx_std_20)
target_link_libraries(game-09-1 PRIVATE engine-09-1)

if(MSVC)
    target_compile_definitions(game-09-1
                               PRIVATE "-DOM_DECLSPEC=__declspec(dllimport)")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # using regular Clang or AppleClang
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()
