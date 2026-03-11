cmake_minimum_required(VERSION 3.22)

add_executable(03-redis-web main.cxx)

target_link_libraries(03-redis-web PRIVATE 
    httplib::httplib
    nlohmann_json::nlohmann_json
    redis++::redis++_static
)
