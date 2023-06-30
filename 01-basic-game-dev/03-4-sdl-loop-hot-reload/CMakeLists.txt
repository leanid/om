cmake_minimum_required(VERSION 3.16...3.23)
project(03-4-sdl-loop-hot-reload)

add_executable(engine-03-4 engine.cxx engine.cxx engine.hxx)
set_target_properties(
    engine-03-4
    PROPERTIES ENABLE_EXPORTS
               ON # we need to be able to link to exe functions from game
    )
if(MINGW OR MSVC)
    # cannot find 01-basic-game-dev/03-4-sdl-loop-hot-reload/libengine-03-4.dll.a:
    # No such file or directory
    target_compile_definitions(
        engine-03-4 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)"
                            "-DGAME_DECLSPEC=__declspec(dllimport)")
else()
    target_compile_definitions(engine-03-4 PRIVATE OM_DECLSPEC= GAME_DECLSPEC=)
endif()

target_compile_features(engine-03-4 PUBLIC cxx_std_20)

target_compile_definitions(
    engine-03-4 PRIVATE $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS=1>)

find_package(SDL3 REQUIRED)
if(MINGW)
    add_custom_command(
        TARGET engine-03-4
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE_DIR:engine-03-4>)
endif()
target_link_libraries(engine-03-4 PRIVATE SDL3::SDL3-shared)

add_library(game-03-4 SHARED game.cxx)

if(MSVC)
    target_compile_definitions(
        game-03-4 PRIVATE "-DOM_DECLSPEC=__declspec(dllimport)"
                          "-DGAME_DECLSPEC=__declspec(dllexport)")
else()
    target_compile_definitions(game-03-4 PRIVATE OM_DECLSPEC= GAME_DECLSPEC= )
endif(MSVC)

target_compile_features(game-03-4 PUBLIC cxx_std_20)
target_link_libraries(game-03-4 PRIVATE engine-03-4)
