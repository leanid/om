cmake_minimum_required(VERSION 3.30)

project(29-localization CXX)

find_package(
    Boost
    # 1.86.0
    # EXACT # Minimum or EXACT version e.g. 1.67.0
    # REQUIRED # Fail with error if Boost is not found
    COMPONENTS locale # Boost libraries by their canonical name
    # e.g. "date_time" for "libboost_date_time"
    #[OPTIONAL_COMPONENTS <libs>...]
    # Optional Boost libraries by their canonical name)
    ) # e.g. "date_time" for "libboost_date_time"

if(NOT Boost_FOUND)
    message(STATUS "Boost and/or required components not found"
                   " skip this example")
    return()
endif()

add_executable(29-localization main.cxx)
target_link_libraries(29-localization PRIVATE Boost::locale)
target_compile_features(29-localization PRIVATE cxx_std_23)
