cmake_minimum_required(VERSION 3.20...3.23)

project(19-boost-file-system CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(
  BOOST_SUBMODULES
  tools/cmake
  libs/align
  libs/assert
  libs/atomic
  libs/concept_check
  libs/config
  libs/container
  libs/container_hash
  libs/conversion
  libs/core
  libs/detail
  libs/filesystem
  libs/function_types
  libs/fusion
  libs/integer
  libs/intrusive
  libs/io
  libs/iterator
  libs/move
  libs/mp11
  libs/mpl
  libs/optional
  libs/predef
  libs/preprocessor
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
)

# The logging output during population can be quite verbose,
# making the configure stage quite noisy. This cache option
# (ON by default) hides all population output unless an error is encountered.
# If experiencing problems with hung downloads, temporarily
# switching this option off may help diagnose which content population
# is causing the issue.
set(FETCHCONTENT_QUIET OFF)

fetchcontent_declare(
  boost
  GIT_REPOSITORY "https://github.com/boostorg/boost.git"
  GIT_TAG "boost-1.79.0"
  GIT_SUBMODULES ${BOOST_SUBMODULES}
  GIT_PROGRESS ON
  GIT_SHALLOW ON
  CONFIGURE_COMMAND "" # faster configure step
)

fetchcontent_makeavailable(boost)

add_executable(main_boost main.cxx)
target_link_libraries(main_boost PRIVATE Boost::filesystem)
