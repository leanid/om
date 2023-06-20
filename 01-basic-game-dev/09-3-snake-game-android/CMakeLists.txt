cmake_minimum_required(VERSION 3.16)

if(SDL3_SRC_DIR)
    add_subdirectory(
        # Specifies the directory of the CMakeLists.txt file.
        ${SDL3_SRC_DIR}
        # Specifies the directory for the build outputs.
        ${CMAKE_CURRENT_BINARY_DIR}/SDL_${ANDROID_ABI})
endif()

project(09-3-start-game-protorype CXX)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    add_library(engine-09-3 SHARED)
else()
    add_executable(engine-09-3)
endif()

target_sources(
    engine-09-3
    PRIVATE om/engine.cxx
            om/engine.hxx
            om/stb_image.h
            om/math.hxx)
set_target_properties(engine-09-3 PROPERTIES ENABLE_EXPORTS TRUE)
target_compile_features(engine-09-3 PRIVATE cxx_std_20)

if(WIN32)
    target_compile_definitions(engine-09-3
                               PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    # find_library(SDL3_LIB NAMES SDL3)
else()
    find_package(SDL3 REQUIRED)
    find_package(OpenGL REQUIRED)
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    # target_include_directories( engine-09-3 PRIVATE
    # ${SDL2_SRC_DIR}/include )
    target_link_libraries(
        engine-09-3
        PRIVATE SDL3::SDL3-shared
                android
                log
                EGL
                GLESv2)
else()
    target_link_libraries(engine-09-3 PRIVATE SDL3::SDL3-shared OpenGL::GL)
endif()

add_library(
    game-09-3 SHARED
    game/configuration_loader.hxx
    game/fruit.cxx
    game/fruit.hxx
    game/game.cxx
    game/game_object.cxx
    game/game_object.hxx
    game/snake.cxx
    game/snake.hxx)

target_include_directories(game-09-3 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(game-09-3 PRIVATE cxx_std_20)

target_link_libraries(game-09-3 PRIVATE engine-09-3)

if(MSVC)
    target_compile_definitions(game-09-3
                               PRIVATE "-DOM_DECLSPEC=__declspec(dllimport)")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # using regular Clang or AppleClang
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()
