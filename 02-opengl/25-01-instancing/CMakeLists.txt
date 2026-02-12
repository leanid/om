cmake_minimum_required(VERSION 3.16.0)

if(SDL3_SRC_DIR)  # Android build directly from source
    add_subdirectory( # Specifies the directory of the CMakeLists.txt file.
                      ${SDL3_SRC_DIR}
                      # Specifies the directory for the build outputs.
                      ${SDL3_BUILD_DIR}_${ANDROID_ABI} )
endif()

# super fast way to go on linux with assimp:
# sudo dnf install assimp-devel
# or
# build it yourself for android for example:
# sudo dnf install glm-devel

project(25-01-instancing CXX)

if (SDL3_SRC_DIR) # Android
    add_library(25-01-instancing SHARED)
    target_include_directories(25-01-instancing PRIVATE ${SDL3_SRC_DIR}/include )
    target_link_libraries(25-01-instancing PRIVATE SDL3::SDL3-shared )
elseif(MINGW)
    add_executable(25-01-instancing)
    find_package(SDL3 REQUIRED)
    find_package(assimp REQUIRED)
    find_package(glm REQUIRED)
    target_link_libraries(25-01-instancing PRIVATE SDL3::SDL3-shared mingw32
                                                      glm
                                                      assimp
                                                      opengl32
                                                      -mwindows
                                                      )
elseif(APPLE)
    add_executable(25-01-instancing)
    find_package(SDL3 CONFIG REQUIRED)
    target_link_libraries(25-01-instancing PRIVATE SDL3::SDL3-shared "-framework OpenGL"
                                              )
elseif(UNIX)
    add_executable(25-01-instancing)
    find_package(SDL3 REQUIRED)
    find_package(assimp REQUIRED)
    find_package(glm REQUIRED)
    target_link_libraries(25-01-instancing PRIVATE SDL3::SDL3-shared ${ASSIMP_LIBRARIES}
                                                  GL
                                                  -lz
                                                  )
    target_include_directories(25-01-instancing PRIVATE ${SDL3_INCLUDE_DIRS})
elseif(MSVC)
    add_executable(25-01-instancing)
    find_package(SDL3 CONFIG REQUIRED)
    find_package(glm CONFIG REQUIRED)
    find_package(assimp REQUIRED)
    target_link_libraries(25-01-instancing PRIVATE SDL3::SDL3-shared glm
                                                  ${ASSIMP_LIBRARIES}
                                                  opengl32
                                                  IrrXMLd.lib
                                                  zlibstaticd.lib
                                                  )
    set(ASSIMP_POSTFIX "-vc140-mtd.lib")
endif()

target_include_directories(25-01-instancing SYSTEM PRIVATE ${CMAKE_BINARY_DIR}/gsl_inc)

target_compile_features(25-01-instancing PUBLIC cxx_std_20)

target_sources(25-01-instancing PRIVATE main.cxx
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

                                   res/runtime.properties.hxx
                                   )

target_link_libraries(25-01-instancing PRIVATE stb::stb)

add_subdirectory(glad)
target_link_libraries(25-01-instancing PRIVATE glad_24_3)

