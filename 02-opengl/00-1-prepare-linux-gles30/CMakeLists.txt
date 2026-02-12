cmake_minimum_required(VERSION 3.12)
project(00-1-prepare-linux-gles30)

add_executable(${PROJECT_NAME} main.cxx)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(${PROJECT_NAME} SDL3::SDL3-shared OpenGL::GL)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /utf-8")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()
