cmake_minimum_required(VERSION 3.12)

if(SDL2_SRC_DIR)  # Android build directly from source
    add_subdirectory( # Specifies the directory of the CMakeLists.txt file.
                      ${SDL2_SRC_DIR}
                      # Specifies the directory for the build outputs.
                      ${SDL2_BUILD_DIR} )  
endif()

project(02-01-shader-uniform CXX)

if (SDL2_SRC_DIR) # Android
    add_library(${PROJECT_NAME} SHARED main.cxx)
    find_library(SDL2_LIB NAMES SDL2)
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_SRC_DIR}/include )
    target_link_libraries(${PROJECT_NAME} SDL2 SDL2main )
elseif(MINGW)
    add_executable(${PROJECT_NAME} main.cxx)
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
    find_package(SDL2 CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE mingw32
                                                  SDL2main
                                                  SDL2
                                                  opengl32
                                                  -mwindows
                                                  )
elseif(APPLE)
    add_executable(${PROJECT_NAME} main.cxx)
    find_package(SDL2 CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main
                                              SDL2::SDL2-static
                                              "-framework OpenGL"
                                              )
elseif(UNIX)
    add_executable(${PROJECT_NAME} main.cxx)
    find_package(SDL2 REQUIRED)
    target_link_libraries(${PROJECT_NAME} SDL2 GL)
elseif(MSVC)
    add_executable(${PROJECT_NAME} main.cxx)
    find_package(SDL2 CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main
                                                  SDL2::SDL2
                                                  opengl32
                                                  )    
endif()

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)


if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /utf-8")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()
