cmake_minimum_required(VERSION 3.9)
project(00-5-prepare-msvc-gles30)

add_executable(${PROJECT_NAME} main.cxx)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

find_package(SDL2 CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main
                                              SDL2::SDL2
                                              opengl32
                                              )

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /std:c++17 /utf-8")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()
