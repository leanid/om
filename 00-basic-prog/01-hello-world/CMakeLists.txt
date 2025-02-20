cmake_minimum_required(VERSION 3.31)

project(01-hello-world LANGUAGES CXX)
# Tell CMake that we explicitly want `import std`. This will initialize the
# property on all targets declared after this to 1
set(CMAKE_CXX_MODULE_STD 1)

include(CheckCXXCompilerFlag)

set(CMAKE_CXX_EXTENSIONS ON) # needed for cxx modules
om_clang_tidy_disable()
if(NOT APPLE)
    add_executable(main_std23 main_std23.cxx)
endif()
om_clang_tidy_enable()

add_executable(hello_cout_dynamic cout_main.cxx)
target_compile_features(hello_cout_dynamic PRIVATE cxx_std_20)

include(CheckIncludeFileCXX)
include(CheckLinkerFlag)

check_include_file_cxx("print" has_cxx_23_print_include)

if(has_cxx_23_print_include)
    add_executable(hello_std23 main_print_std23.cxx)
    target_compile_features(hello_std23 PRIVATE cxx_std_23)
    target_link_libraries(hello_std23
                          PRIVATE $<$<CXX_COMPILER_ID:GNU>:-lstdc++exp>)
else()
    message(STATUS "skip hello_std23 <print> not found")
endif(has_cxx_23_print_include)

message(VERBOSE "${CMAKE_SYSTEM_NAME}")

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(VERBOSE "use gcc/linux specific code here")
    # ~~~
    # install static version of standard libraries
    # >sudo dnf install libstdc++-static glibc-static

    check_cxx_compiler_flag("-static" CXX_SUPPORTS_STATIC_LINKING)
    check_linker_flag(CXX "-static" LINKER_SUPPORTS_STATIC_LINKING)
    message(
        STATUS
            "CXX_SUPPORTS_STATIC_LINKING: ${CXX_SUPPORTS_STATIC_LINKING}\n"
            " LINKER_SUPPORTS_STATIC_LINKING: ${LINKER_SUPPORTS_STATIC_LINKING}"
        )
    if(CXX_SUPPORTS_STATIC_LINKING AND LINKER_SUPPORTS_STATIC_LINKING)
        add_executable(hello_cout_static cout_main.cxx)
        target_link_options(hello_cout_static PRIVATE -static)
        target_compile_features(hello_cout_static PRIVATE cxx_std_20)

        add_executable(hello_nostartfiles hack_main.cxx)
        target_link_options(hello_nostartfiles PRIVATE -nostartfiles)
        target_compile_features(hello_nostartfiles PRIVATE cxx_std_20)

        enable_language(ASM)
        add_executable(hello_asm)
        target_sources(hello_asm PRIVATE syscall_x86_64.s hack_asm_main.cxx)
        target_compile_options(
            hello_asm
            PRIVATE -Os
                    -flto
                    -fno-exceptions
                    -fno-rtti
                    -fno-asynchronous-unwind-tables
                    -Qn)
        target_link_options(
            hello_asm
            PRIVATE
            -flto
            # -dead_strip # not working on clang++
            -z
            noexecstack # fix linking warning on g++13
            -nostartfiles
            -nostdlib)
        # read this if you intresting in binary size:
        # https://www.muppetlabs.com/~breadbox/software/tiny/teensy.html
    else()
        message(
            STATUS "static linking is not supported or you need to install"
                   " libcxx-devel libcxx-static for clang++"
                   " or you need to install libc++-devel libc++-static for g++"
                   " sudo dnf install libstdc++-static glibc-static")
    endif()
endif()

if(UNIX OR MINGW AND NOT APPLE
   )# BONUS for curious (works only in xterm, gnome-terminal or in
    # Konsole
    add_executable(hello_cout_colored cout_main_colored.cxx)
    target_compile_features(hello_cout_colored PRIVATE cxx_std_20)
endif(UNIX OR MINGW)

if(WIN32 AND MSVC)
    add_executable(win32_main WIN32 win32_main.cxx)
endif(WIN32 AND MSVC)
