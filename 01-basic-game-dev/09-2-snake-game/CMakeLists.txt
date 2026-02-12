cmake_minimum_required(VERSION 3.30)
project(09-2-start-game-prototype)

add_executable(
    engine-09-2
    om/engine.cxx
    om/engine.hxx
    om/math.hxx)
set_target_properties(engine-09-2 PROPERTIES ENABLE_EXPORTS TRUE)
target_compile_features(engine-09-2 PRIVATE cxx_std_20)

target_compile_definitions(
    engine-09-2
    PRIVATE "$<$<PLATFORM_ID:Windows>:OM_DECLSPEC=__declspec(dllexport)>")
target_compile_definitions(
    engine-09-2 PRIVATE "$<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_WARNINGS=1>")

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(engine-09-2 PRIVATE SDL3::SDL3-shared OpenGL::GL stb::stb)

# cmake-format: off
add_library(
        game-09-2 SHARED MODULE
        game/configuration_loader.hxx
        game/fruit.cxx
        game/fruit.hxx
        game/game.cxx
        game/game_object.cxx
        game/game_object.hxx
        game/snake.cxx
        game/snake.hxx
)
# cmake-format: on

target_include_directories(game-09-2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(game-09-2 PRIVATE cxx_std_20)
target_link_libraries(game-09-2 PRIVATE engine-09-2)

if(MINGW)
    add_custom_command(
        TARGET engine-09-2
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE_DIR:engine-09-2>)
endif()

target_compile_definitions(
    game-09-2
    PRIVATE "$<$<PLATFORM_ID:Windows>:OM_DECLSPEC=__declspec(dllimport)>")
target_compile_definitions(
    game-09-2
    PRIVATE "$<$<PLATFORM_ID:Windows>:OM_EXPORT=__declspec(dllexport)>")

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND APPLE)
    # using regular Clang or AppleClang
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()
