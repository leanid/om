cmake_minimum_required(VERSION 3.16)
project(08-3-engine-cleanup CXX)

add_executable(engine-08-3 engine.cxx
                           engine.hxx
                           picopng.hxx
                           configuration_loader.hxx
                           game_object.hxx)
set_target_properties(engine-08-3 PROPERTIES ENABLE_EXPORTS TRUE)
target_compile_features(engine-08-3 PUBLIC cxx_std_17)

if(WIN32)
  target_compile_definitions(engine-08-3 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
#find_library(SDL2_LIB NAMES SDL2)

if (MINGW)
    target_link_libraries(engine-08-3
               -lmingw32
               -lSDL2main
               -lSDL2
               -mwindows
               -lopengl32
               )
elseif(MSVC OR UNIX)
  target_link_libraries(engine-08-3 PRIVATE
    SDL2::SDL2
    SDL2::SDL2main
    OpenGL::GL)
endif()

add_library(game-08-3 SHARED game.cxx)
target_compile_features(game-08-3 PUBLIC cxx_std_17)
target_link_libraries(game-08-3 PRIVATE engine-08-3)

if(MSVC)
  target_compile_definitions(game-08-3 PRIVATE "-DOM_DECLSPEC=__declspec(dllimport)")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # using regular Clang or AppleClang
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

install(TARGETS engine-08-3 game-08-3
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)
