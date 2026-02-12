cmake_minimum_required(VERSION 3.16.0)

if(SDL2_SRC_DIR)  # Android build directly from source
    add_subdirectory( # Specifies the directory of the CMakeLists.txt file.
                      ${SDL2_SRC_DIR}
                      # Specifies the directory for the build outputs.
                      ${SDL2_BUILD_DIR}_${ANDROID_ABI} )
endif()

# super fast way to go on linux with assimp:
# sudo dnf install assimp-devel
# or
# build it yourself for android for example:
# sudo dnf install glm-devel

project(26-02-antialising-offscreen CXX)

if (SDL2_SRC_DIR) # Android
    add_library(26-02-antialising-offscreen SHARED)
    find_library(SDL2_LIB NAMES SDL2)
    target_include_directories(26-02-antialising-offscreen PRIVATE ${SDL2_SRC_DIR}/include )
    target_link_libraries(26-02-antialising-offscreen PRIVATE SDL2 SDL2main )
elseif(MINGW)
    add_executable(26-02-antialising-offscreen)
    find_package(SDL2 REQUIRED)
    find_package(assimp REQUIRED)
    find_package(glm REQUIRED)
    target_link_libraries(26-02-antialising-offscreen PRIVATE mingw32
                                                      glm
                                                      assimp
                                                      SDL2::SDL2main
                                                      SDL2::SDL2
                                                      opengl32
                                                      -mwindows
                                                      )
elseif(APPLE)
    add_executable(26-02-antialising-offscreen)
    find_package(SDL2 CONFIG REQUIRED)
    target_link_libraries(15-01-model PRIVATE SDL2::SDL2main
                                              SDL2::SDL2-static
                                              "-framework OpenGL"
                                              )
elseif(UNIX)
    add_executable(26-02-antialising-offscreen)
    find_package(SDL2 REQUIRED)
    find_package(assimp REQUIRED)
    find_package(glm REQUIRED)
    target_link_libraries(26-02-antialising-offscreen PRIVATE SDL2
                                                  ${ASSIMP_LIBRARIES}
                                                  GL
                                                  stdc++fs
                                                  -lz
                                                  )
    target_include_directories(26-02-antialising-offscreen PRIVATE ${SDL2_INCLUDE_DIRS})
elseif(MSVC)
    add_executable(26-02-antialising-offscreen)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(glm CONFIG REQUIRED)
    find_package(assimp REQUIRED)
    target_link_libraries(26-02-antialising-offscreen PRIVATE glm
                                                  ${ASSIMP_LIBRARIES}
                                                  SDL2::SDL2main
                                                  SDL2::SDL2
                                                  opengl32
                                                  IrrXMLd.lib
                                                  zlibstaticd.lib
                                                  )
    set(ASSIMP_POSTFIX "-vc140-mtd.lib")
endif()

target_include_directories(26-02-antialising-offscreen SYSTEM PRIVATE ${CMAKE_BINARY_DIR}/gsl_inc)

target_compile_features(26-02-antialising-offscreen PUBLIC cxx_std_20)

target_sources(26-02-antialising-offscreen PRIVATE main.cxx
                                   gles30_shader.hxx
                                   gles30_shader.cxx
                                   gles30_texture.hxx
                                   gles30_texture.cxx
                                   gles30_mesh.hxx
                                   gles30_mesh.cxx
                                   gles30_model.hxx
                                   gles30_model.cxx
                                   gles30_framebuffer.hxx
                                   gles30_framebuffer.cxx

                                   properties_reader.hxx
                                   properties_reader.cxx
                                   fps_camera.hxx
                                   fps_camera.cxx
                                   opengles30.hxx
                                   opengles30.cxx

                                   res/instanced.vsh
                                   res/instanced.fsh
                                   res/textured.vsh
                                   res/textured.fsh
                                   res/quad.vsh
                                   res/quad.fsh
                                   res/mars.png
                                   res/planet.mtl
                                   res/rock.mtl

                                   res/runtime.properties.hxx
                                   )

target_link_libraries(26-02-antialising-offscreen PRIVATE stb::stb)

add_subdirectory(glad)
target_link_libraries(26-02-antialising-offscreen PRIVATE glad_24_3)

