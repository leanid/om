cmake_minimum_required(VERSION 3.30)

project(aes_128 CXX)

find_package(OpenSSL REQUIRED)
find_package(
    Boost
    # 1.86.0
    # EXACT # Minimum or EXACT version e.g. 1.67.0
    # REQUIRED # Fail with error if Boost is not found
    COMPONENTS program_options # Boost libraries by their canonical name
    REQUIRED
    # e.g. "date_time" for "libboost_date_time"
    #[OPTIONAL_COMPONENTS <libs>...]
    # Optional Boost libraries by their canonical name)
    ) # e.g. "date_time" for "libboost_date_time"

add_executable(aes_128 main.cxx)
target_link_libraries(aes_128 PRIVATE OpenSSL::Crypto Boost::program_options)
target_compile_features(aes_128 PRIVATE cxx_std_23)
