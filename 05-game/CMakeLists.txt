cmake_minimum_required(VERSION 3.19...3.23)
project(match3 LANGUAGES CXX)

# add dependencies (SDL2, boost, CATCH2, dirent and etc)
include(FetchContent)

set(FETCHCONTENT_QUIET OFF) # show log output for fetching

fetchcontent_declare(
  sdl2
  GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
  GIT_TAG "release-2.0.22"
  GIT_PROGRESS ON
  GIT_SHALLOW ON
)

fetchcontent_makeavailable(sdl2)

add_subdirectory(engine)
add_subdirectory(game)
