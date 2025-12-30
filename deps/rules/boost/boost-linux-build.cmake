# 1. how to use:> CXX=clang++ cmake -P boost-linux-build.cmake
cmake_minimum_required(VERSION 4.2)

execute_process(COMMAND ${CMAKE_COMMAND} -B ${CMAKE_CURRENT_LIST_DIR}/build -S ${CMAKE_CURRENT_LIST_DIR} -G Ninja -DCMAKE_CXX_FLAGS="-stdlib=libc++"
                WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                COMMAND_ERROR_IS_FATAL ANY)
execute_process(COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_LIST_DIR}/build --config RelWithDebInfo
                WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                COMMAND_ERROR_IS_FATAL ANY)
