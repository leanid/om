cmake_minimum_required(VERSION 3.31)

project(13-stack-info CXX)
if(WIN32 OR LINUX)
    add_executable(13-stack-info main.cxx stack_info.cxx stack_info.hxx)
    target_link_libraries(13-stack-info PRIVATE $<$<PLATFORM_ID:Linux>:pthread>)
endif()
