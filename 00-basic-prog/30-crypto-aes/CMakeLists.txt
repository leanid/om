cmake_minimum_required(VERSION 3.30)

project(aes128 CXX)

find_package(OpenSSL #REQUIRED
             )
if(NOT OpenSSL_FOUND)
    message(STATUS "OpenSSL not found, skip this example")
    return()
endif()

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
                   " skip this example")
    return()
endif()

add_executable(aes128 main.cxx)
target_link_libraries(aes128 PRIVATE OpenSSL::Crypto Boost::program_options)
target_compile_features(aes128 PRIVATE cxx_std_26)
