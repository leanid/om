cmake_minimum_required(VERSION 3.25)
project(08-3-engine-cleanup CXX)

add_executable(
    engine-08-3
    configuration_loader.hxx
    engine.cxx
    engine.hxx
    game_object.hxx
    picopng.hxx)
set_target_properties(engine-08-3 PROPERTIES ENABLE_EXPORTS TRUE)
target_compile_features(engine-08-3 PUBLIC cxx_std_20)

if(WIN32)
    target_compile_definitions(engine-08-3
                               PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_package(OpenGL REQUIRED)
find_package(SDL3 REQUIRED)

target_link_libraries(engine-08-3 PRIVATE SDL3::SDL3-shared OpenGL::GL)

add_library(game-08-3 SHARED MODULE game.cxx)
target_compile_features(game-08-3 PUBLIC cxx_std_20)
target_link_libraries(game-08-3 PRIVATE engine-08-3)

if(MSVC)
    target_compile_definitions(game-08-3
                               PRIVATE "-DOM_DECLSPEC=__declspec(dllimport)")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # using regular Clang or AppleClang
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()
