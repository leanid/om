# 1. how to use:> CXX=clang++ cmake -P linux-build.cmake
cmake_minimum_required(VERSION 4.2)
# edit flags as you need
set(CMAKE_CXX_FLAGS "-stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS "-L/usr/lib64/gcc/x86_64-alt-linux/13/ -B/usr/lib64/gcc/x86_64-alt-linux/13/")
set(CMAKE_SHARED_LINKER_FLAGS "-L/usr/lib64/gcc/x86_64-alt-linux/13/ -B/usr/lib64/gcc/x86_64-alt-linux/13/")

execute_process(COMMAND ${CMAKE_COMMAND} -B ${CMAKE_CURRENT_LIST_DIR}/build -S ${CMAKE_CURRENT_LIST_DIR}
                        -G Ninja
                        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
                        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
                        -DCMAKE_LINKER_TYPE=LLD
                        -DCMAKE_BUILD_TYPE=RelWithDebInfo
                        -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
                        -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
                WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                COMMAND_ERROR_IS_FATAL ANY)
execute_process(COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_LIST_DIR}/build --config RelWithDebInfo
                WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                COMMAND_ERROR_IS_FATAL ANY)
