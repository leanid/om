cmake_minimum_required(VERSION 3.22)

project(all-curses CXX)

set(OM_BUILD_CURSES OFF CACHE BOOL "turn ON if need to build curses")

# curses
if(${OM_BUILD_CURSES})
  add_subdirectory(00-basic-prog)
  add_subdirectory(01-basic-game-dev)
  add_subdirectory(02-graphics-3d)
endif(${OM_BUILD_CURSES})

# start sample project
add_subdirectory(05-game)
