cmake_minimum_required(VERSION 4.2)

add_executable(03-redis-web main.cxx)

target_link_libraries(03-redis-web PRIVATE 
    httplib::httplib
    nlohmann_json::nlohmann_json
    redis++::redis++_static
)

# Copy public folder and config to build directory
# file(COPY public DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
# file(COPY config.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
