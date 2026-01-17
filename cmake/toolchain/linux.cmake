# on Alt Linux to build with downloaded latest llvm we need to pass search path for
# objects and libraries path for linker to find crt on current system
# 1. Ask the system C compiler for the full path to libgcc.a
execute_process(
    COMMAND gcc -print-libgcc-file-name
    OUTPUT_VARIABLE libgcc_path
    OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY
)

# 2. Get the directory containing that file (this directory also contains crtbegin.o)
get_filename_component(gcc_libs_and_obj_dir "${libgcc_path}" DIRECTORY)

# 3. Apply the flags using the discovered path
if(NOT gcc_libs_and_obj_dir)
    message(FATAL "Could not determine GCC internal library path.")
endif()

# Set linker flags for executables, so etc
set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} -L${gcc_libs_and_obj_dir} -B${gcc_libs_and_obj_dir}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -L${gcc_libs_and_obj_dir} -B${gcc_libs_and_obj_dir}")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -L${gcc_libs_and_obj_dir} -B${gcc_libs_and_obj_dir}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
