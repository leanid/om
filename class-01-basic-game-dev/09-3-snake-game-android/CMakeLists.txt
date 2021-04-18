cmake_minimum_required(VERSION 3.16)

if(SDL2_SRC_DIR)
    add_subdirectory( # Specifies the directory of the CMakeLists.txt file.
                     ${SDL2_SRC_DIR}
                     # Specifies the directory for the build outputs.
                     ${SDL2_BUILD_DIR}_${ANDROID_ABI}
                    )
endif()


project(09-3-start-game-protorype CXX)

if (SDL2_SRC_DIR)
    add_library(engine-09-3 SHARED)
else()
    add_executable(engine-09-3)
endif()
target_sources(engine-09-3 PRIVATE om/engine.cxx
                                   om/engine.hxx
                                   om/stb_image.h
                                   om/math.hxx)
set_target_properties(engine-09-3 PROPERTIES ENABLE_EXPORTS TRUE)
target_compile_features(engine-09-3 PRIVATE cxx_std_20)

if(WIN32)
  target_compile_definitions(engine-09-3 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_package(SDL2 REQUIRED)
find_library(SDL2_LIB NAMES SDL2)
if (NOT SDL2_SRC_DIR)
    find_package(OpenGL REQUIRED)
endif()

if (MINGW)
    find_package(sdl2 REQUIRED)
    target_link_libraries(engine-09-3 PRIVATE
                          mingw32
			  SDL2::SDL2main
			  SDL2::SDL2
                          opengl32
			  -mwindows)
elseif(UNIX)
    if(SDL2_SRC_DIR)
        #target_include_directories( engine-09-3 PRIVATE ${SDL2_SRC_DIR}/include )
        target_link_libraries( engine-09-3 PRIVATE
            SDL2
            SDL2main
            android
            log
            EGL
            GLESv2
            )
    else()
        target_link_libraries(engine-09-3 PRIVATE
                SDL2::SDL2
                SDL2::SDL2main
                OpenGL::GL
                )
    endif()
elseif(MSVC)
    find_package(sdl2 REQUIRED)
    target_link_libraries(engine-09-3 PRIVATE SDL2::SDL2 SDL2::SDL2main
                          opengl32)
endif()

add_library(game-09-3 SHARED game/game.cxx 
                        game/game_object.cxx
                        game/snake.cxx
                        game/fruit.cxx
                        game/configuration_loader.hxx
                        game/game_object.hxx
                        game/snake.hxx
                        game/fruit.hxx)
target_include_directories(game-09-3 PRIVATE .)
target_compile_features(game-09-3 PRIVATE cxx_std_20)

target_link_libraries(game-09-3 PRIVATE engine-09-3)

if(MSVC)
  target_compile_definitions(game-09-3 PRIVATE "-DOM_DECLSPEC=__declspec(dllimport)")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # using regular Clang or AppleClang
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

install(TARGETS engine-09-3 game-09-3
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)
