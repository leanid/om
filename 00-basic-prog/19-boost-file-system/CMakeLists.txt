cmake_minimum_required(VERSION 3.27...3.29)

project(19-boost-file-system CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# The logging output during population can be quite verbose,
# making the configure stage quite noisy. This cache option
# (ON by default) hides all population output unless an error is encountered.
# If experiencing problems with hung downloads, temporarily
# switching this option off may help diagnose which content population
# is causing the issue.
set(FETCHCONTENT_QUIET OFF)

set(BOOST_INCLUDE_LIBRARIES filesystem)

fetchcontent_declare(
    Boost
    GIT_REPOSITORY "https://github.com/boostorg/boost.git"
    GIT_TAG "boost-1.85.0"
    # list of all deps for filesystem submodule
    GIT_SUBMODULES
        libs/assert
        libs/bind
        libs/concept_check
        libs/config
        libs/container_hash
        libs/core
        libs/describe
        libs/detail
        libs/filesystem
        libs/function
        libs/function_types
        libs/functional
        libs/fusion
        libs/io
        libs/iterator
        libs/move
        libs/mp11
        libs/mpl
        libs/optional
        libs/predef
        libs/preprocessor
        libs/scope
        libs/smart_ptr
        libs/static_assert
        libs/system
        libs/throw_exception
        libs/tuple
        libs/type_traits
        libs/typeof
        libs/utility
        libs/variant2
        libs/winapi
        tools/cmake
    GIT_PROGRESS ON
    GIT_SHALLOW ON)

fetchcontent_makeavailable(Boost)

add_executable(main_boost main.cxx)
target_link_libraries(main_boost PRIVATE Boost::filesystem)
