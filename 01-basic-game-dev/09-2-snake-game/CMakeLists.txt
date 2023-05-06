cmake_minimum_required(VERSION 3.16)
project(09-2-start-game-protorype)

add_executable(
    engine-09-2
    om/engine.cxx
    om/engine.hxx
    om/math.hxx
    om/stb_image.h)
set_target_properties(engine-09-2 PROPERTIES ENABLE_EXPORTS TRUE)
target_compile_features(engine-09-2 PRIVATE cxx_std_20)

if(WIN32)
    target_compile_definitions(engine-09-2
                               PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(engine-09-2 PRIVATE SDL3::SDL3-shared OpenGL::GL)

add_library(
    game-09-2 SHARED
    game/configuration_loader.hxx
    game/fruit.cxx
    game/fruit.hxx
    game/game.cxx
    game/game_object.cxx
    game/game_object.hxx
    game/snake.cxx
    game/snake.hxx)
target_include_directories(game-09-2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(game-09-2 PRIVATE cxx_std_20)
target_link_libraries(game-09-2 PRIVATE engine-09-2)

if(MSVC)
    target_compile_definitions(game-09-2
                               PRIVATE "-DOM_DECLSPEC=__declspec(dllimport)")
    target_compile_definitions(game-09-2
                               PRIVATE "-DOM_EXPORT=__declspec(dllexport)")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # using regular Clang or AppleClang
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

install(TARGETS engine-09-2 game-09-2
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)
