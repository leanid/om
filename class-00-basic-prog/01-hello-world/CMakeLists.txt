cmake_minimum_required(VERSION 3.14)

add_executable(hello_cout_dynamic cout_main.cxx)

# install static version of standard libraries
#>sudo dnf install libstdc++-static glibc-static
add_executable(hello_cout_static cout_main.cxx)
target_link_libraries(hello_cout_static -static)

add_executable(hello_nostartfiles hack_main.cxx)
target_link_options(hello_nostartfiles PRIVATE -nostartfiles)

enable_language(ASM)
add_executable(hello_asm hack_asm_main.cxx syscall.s)
target_link_options(hello_asm PRIVATE -nostartfiles -nostdlib)
