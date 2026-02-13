# 1. how to use: cmake -P windows-build.cmake
#    With clang-cl (recommended): add LLVM to PATH, set CC=clang CXX=clang++
#    With MSVC: run from "x64 Native Tools Command Prompt for VS"
#    Set OM_USE_LLD=0 to skip LLD when using MSVC without LLVM
cmake_minimum_required(VERSION 4.2)

get_filename_component(
    toolchain_path
    "${CMAKE_CURRENT_LIST_DIR}/../../cmake/toolchain/windows.cmake" ABSOLUTE)

execute_process(
    COMMAND
        ${CMAKE_COMMAND} -B ${CMAKE_CURRENT_LIST_DIR}/build -S
        ${CMAKE_CURRENT_LIST_DIR} -G Ninja
        -DCMAKE_TOOLCHAIN_FILE=${toolchain_path}
        -DCMAKE_BUILD_TYPE=RelWithDebInfo
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR} COMMAND_ERROR_IS_FATAL ANY)

execute_process(
    COMMAND
        ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_LIST_DIR}/build --config
        RelWithDebInfo
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR} COMMAND_ERROR_IS_FATAL ANY)
