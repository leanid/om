cmake_minimum_required(VERSION 3.16...3.23)

project(13-stack-info CXX)
if (WIN32 OR LINUX)
    add_executable(13-stack-info main.cxx stack_info.hxx stack_info.cxx)
    target_compile_features(13-stack-info PRIVATE cxx_std_20)
    target_link_libraries(13-stack-info PRIVATE $<$<PLATFORM_ID:Linux>:pthread>)
endif()
