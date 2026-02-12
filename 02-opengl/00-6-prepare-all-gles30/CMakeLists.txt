cmake_minimum_required(VERSION 3.12)

if(SDL3_SRC_DIR)  # Android build directly from source
    set(SDL_DISABLE_IMMINTRIN_H 1)
    add_subdirectory( # Specifies the directory of the CMakeLists.txt file.
                      ${SDL3_SRC_DIR}
                      # Specifies the directory for the build outputs.
                      ${SDL3_BUILD_DIR} )
endif()

project(00-6-prepare-all-gles30 CXX)

if (SDL3_SRC_DIR) # Android
    add_library(${PROJECT_NAME} SHARED main.cxx)
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL3_SRC_DIR}/include )
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-shared )
elseif(MINGW)
    add_executable(${PROJECT_NAME} main.cxx)
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
    find_package(SDL3 CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE mingw32
                                                  SDL3::SDL3-shared
                                                  opengl32
                                                  -mwindows
                                                  )
elseif(APPLE)
    add_executable(${PROJECT_NAME} main.cxx)
    find_package(SDL3 CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-shared
                                              "-framework OpenGL"
                                              )
elseif(UNIX)
    add_executable(${PROJECT_NAME} main.cxx)
    find_package(SDL3 REQUIRED)
    find_package(OpenGL REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-shared
        OpenGL::GL)
elseif(MSVC)
    add_executable(${PROJECT_NAME} main.cxx)
    find_package(SDL3 CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-shared
                                                  opengl32
                                                  )
endif()

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)


if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /utf-8")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()
