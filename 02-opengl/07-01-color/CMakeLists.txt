cmake_minimum_required(VERSION 3.12)

if(SDL3_SRC_DIR)  # Android build directly from source
    add_subdirectory( # Specifies the directory of the CMakeLists.txt file.
                      ${SDL3_SRC_DIR}
                      # Specifies the directory for the build outputs.
                      ${SDL3_BUILD_DIR}_${ANDROID_ABI} )
endif()

project(07-01-color CXX)

if (SDL3_SRC_DIR) # Android
    add_library(${PROJECT_NAME} SHARED)
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL3_SRC_DIR}/include )
    target_link_libraries(${PROJECT_NAME} SDL3::SDL3-shared )
elseif(MINGW)
    add_executable(${PROJECT_NAME})
    find_package(SDL3 CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-shared mingw32
                                                  opengl32
                                                  -mwindows
                                                  )
elseif(APPLE)
    add_executable(${PROJECT_NAME})
    find_package(SDL3 CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-shared "-framework OpenGL"
                                              )
elseif(UNIX)
    add_executable(${PROJECT_NAME})
    find_package(SDL3 REQUIRED)
    target_link_libraries(${PROJECT_NAME} GL stdc++fs)
elseif(MSVC)
    add_executable(${PROJECT_NAME})
    find_package(SDL3 CONFIG REQUIRED)
    find_package(glm CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-shared glm
                                                  opengl32
                                                  )
endif()

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_sources(${PROJECT_NAME} PRIVATE main.cxx
                                       gles30_shader.cxx
                                       opengles30.cxx
                                       gles30_texture.cxx
                                       properties_reader.cxx
                                       fps_camera.cxx
                                       opengles30.hxx
                                       gles30_shader.hxx
                                       gles30_texture.hxx
                                       properties_reader.hxx
                                       fps_camera.hxx)

target_link_libraries(07-01-color PRIVATE stb::stb)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4  /utf-8 ") # /WX
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror -Wno-unknown-pragmas")
endif()
