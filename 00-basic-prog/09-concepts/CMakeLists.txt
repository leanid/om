cmake_minimum_required(VERSION 3.16)

project(09-concepts CXX)
# tested only for g++ compiler
if(CMAKE_COMPILER_IS_GNUCXX)
    if(CMAKE_COMPILER_IS_GNUCXX
       AND CMAKE_CXX_COMPILER_VERSION
           VERSION_LESS
           10.1.0
    )
        message(STATUS "Require at least gcc-10.1")
    else()
        add_executable(
            09-concepts
            "main.cxx"
        )
        target_compile_features(
            09-concepts
            PRIVATE cxx_std_20
        )
        target_compile_options(
            09-concepts
            PRIVATE -fconcepts
        )
    endif()
endif()
