cmake_minimum_required(VERSION 3.30)

project(aes_128 CXX)

find_package(OpenSSL CONFIG REQUIRED)

add_executable(aes_128 main.cxx)
target_link_libraries(aes_128 PRIVATE OpenSSL::Crypto)
target_compile_features(aes_128 PRIVATE cxx_std_23)
