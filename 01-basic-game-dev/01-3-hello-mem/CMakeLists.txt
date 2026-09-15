cmake_minimum_required(VERSION 3.22)

# Все три под-примера Linux-only: /proc/self/maps, sbrk, mmap/munmap,
# интерпозиция символов malloc/new, dlopen.
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "01-3-hello-mem: examples are Linux-only, skipping")
    return()
endif()

project(01-3-hello-mem CXX)

# Трилогия про память процесса (порядок учебный - смотреть по очереди):
add_subdirectory(layout) # 1. из чего состоит память процесса изнутри
add_subdirectory(mem)    # 2. запрет new/malloc + свой mmap-аллокатор
add_subdirectory(libs)   # 3. запрет vs static/shared/dlopen библиотеки
