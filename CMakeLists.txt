cmake_minimum_required(VERSION 3.22...3.24)

project(gamedev_study CXX)

set(OM_BUILD_COURSES OFF CACHE BOOL "turn ON if need to build courses")

# curses
if(${OM_BUILD_COURSES})
  add_subdirectory(00-basic-prog)
  add_subdirectory(01-basic-game-dev)
  add_subdirectory(02-graphics-3d)
endif(${OM_BUILD_COURSES})

# start sample project
add_subdirectory(05-game)
