add_executable(41-redis-log-client main.cxx)

target_link_libraries(41-redis-log-client PRIVATE redis++::redis++_static)
