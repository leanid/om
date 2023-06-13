cmake_minimum_required(VERSION 3.22)
project(hello-lib CXX)

# select SHARED or STATIC library
add_library(hello-lib-static STATIC include/hello_lib.hxx src/hello_lib.cxx)
target_include_directories(hello-lib-static
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(hello-lib-static PUBLIC cxx_std_20)

add_library(hello-lib-dynamic SHARED include/hello_lib.hxx src/hello_lib.cxx)
set_target_properties(hello-lib-dynamic PROPERTIES ENABLE_EXPORTS TRUE)

if(MSVC)
    target_compile_definitions(hello-lib-dynamic
                               PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(MSVC)

target_include_directories(hello-lib-dynamic
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(hello-lib-dynamic PUBLIC cxx_std_20)

