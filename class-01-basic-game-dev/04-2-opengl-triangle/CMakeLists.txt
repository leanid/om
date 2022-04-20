cmake_minimum_required(VERSION 3.16..3.22)
project(04-2-opengl-triangle)

add_library(engine-04-2 SHARED engine.cxx engine.hxx)
target_compile_features(engine-04-2 PUBLIC cxx_std_17)

if(WIN32)
    target_compile_definitions(engine-04-2
                               PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_package(SDL2 REQUIRED)

target_link_libraries(engine-04-2 PRIVATE SDL2::SDL2 SDL2::SDL2main)

# if you want to link with OpenGL library yourself you can skip glad.{h,c} from
# previous example and directly add link libraries for your platform
if(MINGW)
    target_link_libraries(
        engine-04-2
        PRIVATE -lmingw32
                -lSDL2main
                -lSDL2
                -mwindows
                -lopengl32)
elseif(APPLE)
    target_link_libraries(engine-04-2 PRIVATE -Wl,-framework,OpenGL)
elseif(UNIX)
    target_link_libraries(engine-04-2 PRIVATE GL)
elseif(MSVC)
    target_link_libraries(engine-04-2 PRIVATE opengl32)
endif()

add_executable(game-04-2 game.cxx)
target_compile_features(game-04-2 PUBLIC cxx_std_17)
target_link_libraries(game-04-2 PRIVATE engine-04-2)

install(TARGETS engine-04-2 game-04-2
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)
