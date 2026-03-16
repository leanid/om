cmake_minimum_required(VERSION 4.0)

set(android_toolchain_file "$ENV{ANDROID_NDK}/build/cmake/android.toolchain.cmake")

foreach(abi IN ITEMS x86_64 x86 arm64-v8a armeabi-v7a)
    set(CMAKE_PREFIX_PATH)
    execute_process(
        COMMAND
            ${CMAKE_COMMAND}
            -E remove_directory build_${abi}
        COMMAND_ECHO STDOUT
        COMMAND_ERROR_IS_FATAL ANY)
    execute_process(
        COMMAND
            ${CMAKE_COMMAND}
            -S ${CMAKE_CURRENT_LIST_DIR}
            -B build_${abi}
            -G "Ninja"
            -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
            -DCMAKE_TOOLCHAIN_FILE=${android_toolchain_file}
            -DANDROID_ABI=${abi}
            -DCMAKE_ANDROID_ARCH_ABI=${abi}
            -DANDROID_PLATFORM=android-21
        WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
        COMMAND_ECHO STDOUT
        COMMAND_ERROR_IS_FATAL ANY)
    execute_process(
        COMMAND
            ${CMAKE_COMMAND}
            --build build_${abi}
            --config RelWithDebInfo
        WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
        COMMAND_ECHO STDOUT
        COMMAND_ERROR_IS_FATAL ANY)
endforeach()
