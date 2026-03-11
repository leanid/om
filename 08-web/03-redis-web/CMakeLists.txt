cmake_minimum_required(VERSION 3.22)

project(03-redis-web CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Fetch cpp-httplib
include(FetchContent)
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.15.3
)
FetchContent_MakeAvailable(httplib)

# Fetch nlohmann_json
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# Find redis++
find_package(redis++ REQUIRED)

add_executable(03-redis-web main.cxx)

target_link_libraries(03-redis-web PRIVATE
    httplib::httplib
    nlohmann_json::nlohmann_json
    redis++::redis++_static
)

# Copy public folder to build directory
file(COPY public DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
