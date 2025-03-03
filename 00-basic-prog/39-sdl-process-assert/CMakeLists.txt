cmake_minimum_required(VERSION 3.31)

project(39-sdl-process-assert)

find_package(SDL3 REQUIRED)
find_package(
    Boost 1.86.0 # same version with same compiler settings
    EXACT # Minimum or EXACT version e.g. 1.67.0
    # REQUIRED # Fail with error if Boost is not found
    COMPONENTS program_options # Boost libraries by their canonical name
    # REQUIRED
    # e.g. "date_time" for "libboost_date_time"
    #[OPTIONAL_COMPONENTS <libs>...]
    # Optional Boost libraries by their canonical name)
    ) # e.g. "date_time" for "libboost_date_time"

if(NOT Boost_FOUND)
    message(STATUS "Boost and/or required components not found"
                   " skip 39-sdl-process-assert")
    return()
endif()

add_executable(39-sdl-process-assert WIN32 main.cxx show_message.cxx
                                           show_message.hxx)

target_link_libraries(39-sdl-process-assert PRIVATE SDL3::SDL3
                                                    Boost::program_options)
