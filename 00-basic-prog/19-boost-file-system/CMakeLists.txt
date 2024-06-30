cmake_minimum_required(VERSION 3.27...3.29)

project(19-boost-file-system CXX)

include(FetchContent)

# The logging output during population can be quite verbose,
# making the configure stage quite noisy. This cache option
# (ON by default) hides all population output unless an error is encountered.
# If experiencing problems with hung downloads, temporarily
# switching this option off may help diagnose which content population
# is causing the issue.
set(FETCHCONTENT_QUIET OFF)

fetchcontent_declare(
    Boost
    GIT_REPOSITORY "https://github.com/boostorg/boost.git"
    GIT_TAG "boost-1.85.0"
    GIT_PROGRESS ON
    GIT_SHALLOW ON)

fetchcontent_makeavailable(Boost)

find_package(boost_filesystem CONFIG REQUIRED)

add_executable(main_boost main.cxx)

target_link_libraries(main_boost PRIVATE Boost::filesystem)
