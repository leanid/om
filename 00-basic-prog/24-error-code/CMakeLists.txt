cmake_minimum_required(VERSION 3.26)

project(24-error-code CXX)

include(CheckCXXSymbolExists)

check_cxx_symbol_exists(__cpp_lib_ranges_zip "ranges" cxx_have_ranges_zip)

if(cxx_have_ranges_zip)
    add_executable(24-error-code main.cxx)
    target_compile_features(24-error-code PRIVATE cxx_std_23)
else()
    message(STATUS "skip 24-error-code no std::ranges::zip_transform")
endif(cxx_have_ranges_zip)
