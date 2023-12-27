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

project(24-02-geo-explode CXX)

if (SDL2_SRC_DIR) # Android
    add_library(24-02-geo-explode SHARED)
    find_library(SDL2_LIB NAMES SDL2)
    target_include_directories(24-02-geo-explode PRIVATE ${SDL2_SRC_DIR}/include )
    target_link_libraries(24-02-geo-explode PRIVATE SDL2 SDL2main )
elseif(MINGW)
    add_executable(24-02-geo-explode)
    find_package(SDL2 REQUIRED)
    find_package(assimp REQUIRED)
    find_package(glm REQUIRED)
    target_link_libraries(24-02-geo-explode PRIVATE mingw32
                                                      glm
                                                      assimp
                                                      SDL2::SDL2main
                                                      SDL2::SDL2
                                                      opengl32
                                                      -mwindows
                                                      )
elseif(APPLE)
    add_executable(24-02-geo-explode)
    find_package(SDL2 CONFIG REQUIRED)
    target_link_libraries(15-01-model PRIVATE SDL2::SDL2main
                                              SDL2::SDL2-static
                                              "-framework OpenGL"
                                              )
elseif(UNIX)
    add_executable(24-02-geo-explode)
    find_package(SDL2 REQUIRED)
    find_package(assimp REQUIRED)
    find_package(glm REQUIRED)
    target_link_libraries(24-02-geo-explode PRIVATE SDL2::SDL2 SDL2::SDL2main
                                                  ${ASSIMP_LIBRARIES}
                                                  GL
                                                  stdc++fs
                                                  -lz
                                                  )
    target_include_directories(24-02-geo-explode PRIVATE ${SDL2_INCLUDE_DIRS})
elseif(MSVC)
    add_executable(24-02-geo-explode)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(glm CONFIG REQUIRED)
    find_package(assimp REQUIRED)
    target_link_libraries(24-02-geo-explode PRIVATE glm
                                                  ${ASSIMP_LIBRARIES}
                                                  SDL2::SDL2main
                                                  SDL2::SDL2
                                                  opengl32
                                                  IrrXMLd.lib
                                                  zlibstaticd.lib
                                                  )
    set(ASSIMP_POSTFIX "-vc140-mtd.lib")
endif()

target_include_directories(24-02-geo-explode SYSTEM PRIVATE ${CMAKE_BINARY_DIR}/gsl_inc)

target_compile_features(24-02-geo-explode PUBLIC cxx_std_20)

target_sources(24-02-geo-explode PRIVATE main.cxx
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

                                   stb_image.h

                                   res/explode.vsh
                                   res/explode.gsh
                                   res/explode.fsh

                                   res/runtime.properties.hxx
                                   )

add_subdirectory(glad)
target_link_libraries(24-02-geo-explode PRIVATE glad_24_2)

