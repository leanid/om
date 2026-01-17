# 1. how to use:> CXX=clang++ cmake -P linux-build.cmake
cmake_minimum_required(VERSION 4.2)

get_filename_component(toolchain_path "${CMAKE_CURRENT_LIST_DIR}/../../cmake/toolchain/linux.cmake" ABSOLUTE)

execute_process(COMMAND ${CMAKE_COMMAND} -B ${CMAKE_CURRENT_LIST_DIR}/build -S ${CMAKE_CURRENT_LIST_DIR}
                        -G Ninja
                        -DCMAKE_TOOLCHAIN_FILE=${toolchain_path}
                        -DCMAKE_LINKER_TYPE=LLD
                        -DCMAKE_BUILD_TYPE=RelWithDebInfo
                WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                COMMAND_ERROR_IS_FATAL ANY)
execute_process(COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_LIST_DIR}/build --config RelWithDebInfo
                WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                COMMAND_ERROR_IS_FATAL ANY)
