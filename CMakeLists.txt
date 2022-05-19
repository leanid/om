cmake_minimum_required(VERSION 3.22)

project(all-curses CXX)

# my curses
add_subdirectory(00-basic-prog)
add_subdirectory(01-basic-game-dev)
add_subdirectory(02-graphics-3d)

# my start sample project
add_subdirectory(game)
