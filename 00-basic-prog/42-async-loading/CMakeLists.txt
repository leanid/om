find_package(Boost COMPONENTS chrono thread REQUIRED)

add_executable(42-async-loading std_main.cxx)

add_executable(42-async-loading-boost boost_main.cpp)
target_link_libraries(42-async-loading-boost PRIVATE Boost::chrono
                                                     Boost::thread)
