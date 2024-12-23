cmake_minimum_required(VERSION 3.30)

project(36-gtest)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
fetchcontent_declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
fetchcontent_makeavailable(googletest)

enable_testing()

add_executable(36-gtest main.cxx)
target_link_libraries(36-gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(36-gtest)
