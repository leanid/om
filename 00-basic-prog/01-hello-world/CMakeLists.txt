cmake_minimum_required(VERSION 3.30)

project(01-hello-world LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS ON) # needed for cxx modules

add_executable(hello_cout_dynamic cout_main.cxx)
target_compile_features(hello_cout_dynamic PRIVATE cxx_std_20)

include(CheckIncludeFileCXX)
check_include_file_cxx("print" has_cxx_23_print_include)

if(has_cxx_23_print_include)
    add_executable(hello_std23 main_print_std23.cxx)
    target_compile_features(hello_std23 PRIVATE cxx_std_23)
else()
    message(STATUS "skip hello_std23 <print> not found")
endif(has_cxx_23_print_include)

message(VERBOSE "${CMAKE_SYSTEM_NAME}")

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(VERBOSE "use gcc/linux specific code here")
    # ~~~
    # install static version of standard libraries
    # >sudo dnf install libstdc++-static glibc-static
    add_executable(hello_cout_static cout_main.cxx)
    target_link_options(hello_cout_static PRIVATE -static)
    target_compile_features(hello_cout_static PRIVATE cxx_std_20)

    add_executable(hello_nostartfiles hack_main.cxx)
    target_link_options(hello_nostartfiles PRIVATE -nostartfiles)
    target_compile_features(hello_nostartfiles PRIVATE cxx_std_20)

    enable_language(ASM)
    add_executable(hello_asm)
    target_sources(hello_asm PRIVATE syscall_x86_64.s hack_asm_main.cxx)
    target_link_options(
        hello_asm
        PRIVATE
        -z
        noexecstack # fix linking warning on g++13
        -nostartfiles
        -nostdlib)
endif()

if(UNIX OR MINGW) # BONUS for curious (works only in xterm, gnome-terminal or in
                  # Konsole
    add_executable(hello_cout_colored cout_main_colored.cxx)
    target_compile_features(hello_cout_colored PRIVATE cxx_std_20)
endif(UNIX OR MINGW)

if(WIN32)
    add_executable(win32_main WIN32 win32_main.cxx)
endif(WIN32)
