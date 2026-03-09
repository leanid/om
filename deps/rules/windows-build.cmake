# 1. how to use: cmake -P windows-build.cmake
#    With clang-cl (recommended): add LLVM to PATH, set CC=clang CXX=clang++
#    With MSVC: run from "x64 Native Tools Command Prompt for VS"
#    Set OM_USE_LLD=0 to skip LLD when using MSVC without LLVM
# 2. on PowerShell set environment vars like:
#    $ENV:CC = "clang"
#    $ENV:CXX = "clang++"
cmake_minimum_required(VERSION 4.2)

get_filename_component(
    toolchain_path
    "${CMAKE_CURRENT_LIST_DIR}/../../cmake/toolchain/windows.cmake" ABSOLUTE)

# on windows several compilers can be used:
# 1. Ninja - -G Ninja (with CC=clang CXX=clang++)
#   (remove MSYS2/MinGW from path to stom conflicting msvc runtime with mingw64)
# 2. MSVC - -G "Visual Studio 17 2022" -T v143
# 3. MSVC - -G "Visual Studio 18 2026" -T v145

execute_process(
    COMMAND
        ${CMAKE_COMMAND} -B ${CMAKE_CURRENT_LIST_DIR}/build -S
        ${CMAKE_CURRENT_LIST_DIR} -G "Ninja"
        -DCMAKE_TOOLCHAIN_FILE=${toolchain_path}
        -DCMAKE_BUILD_TYPE=RelWithDebInfo --fresh
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR} COMMAND_ERROR_IS_FATAL ANY)

execute_process(
    COMMAND
        ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_LIST_DIR}/build --config
        RelWithDebInfo
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR} COMMAND_ERROR_IS_FATAL ANY)
