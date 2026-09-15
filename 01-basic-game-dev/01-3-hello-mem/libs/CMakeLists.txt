cmake_minimum_required(VERSION 3.22)

project(01-3-mem-libs CXX)

add_subdirectory(greedy_lib)

# ВАЖНО: banned.cxx компилируется прямо в каждый исполняемый файл, а не
# складывается в статическую библиотеку. Из .a линкер вытаскивает
# объектник только ради неразрешённых символов, а malloc/operator new уже
# "разрешены" из libc/libstdc++ - и запрещающие определения молча не
# попали бы в бинарь.

add_executable(01-3-mem-libs-static app_linked.cxx banned.cxx banned.hxx
                                    maps.cxx maps.hxx demo.cxx demo.hxx)
target_compile_features(01-3-mem-libs-static PUBLIC cxx_std_20)
target_compile_definitions(01-3-mem-libs-static PRIVATE VARIANT="static")
target_link_libraries(01-3-mem-libs-static PRIVATE greedy-lib-static)

add_executable(01-3-mem-libs-shared app_linked.cxx banned.cxx banned.hxx
                                    maps.cxx maps.hxx demo.cxx demo.hxx)
target_compile_features(01-3-mem-libs-shared PUBLIC cxx_std_20)
target_compile_definitions(01-3-mem-libs-shared PRIVATE VARIANT="shared")
target_link_libraries(01-3-mem-libs-shared PRIVATE greedy-lib-shared)

add_executable(01-3-mem-libs-dlopen app_dlopen.cxx banned.cxx banned.hxx
                                    maps.cxx maps.hxx demo.cxx demo.hxx)
target_compile_features(01-3-mem-libs-dlopen PUBLIC cxx_std_20)
target_link_libraries(01-3-mem-libs-dlopen PRIVATE ${CMAKE_DL_LIBS})
target_compile_definitions(
    01-3-mem-libs-dlopen
    PRIVATE "GREEDY_SO_PATH=\"$<TARGET_FILE:greedy-lib-shared>\"")
add_dependencies(01-3-mem-libs-dlopen greedy-lib-shared)

include(CTest)

add_test(NAME check_mem_libs_static_exit_zero COMMAND 01-3-mem-libs-static)
add_test(NAME check_mem_libs_shared_exit_zero COMMAND 01-3-mem-libs-shared)
add_test(NAME check_mem_libs_dlopen_exit_zero COMMAND 01-3-mem-libs-dlopen)
