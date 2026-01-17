# on Alt Linux to build with downloaded latest llvm we need to pass search path for
# objects and libraries path for linker to find crt on current system
set(CMAKE_EXE_LINKER_FLAGS "-L/usr/lib64/gcc/x86_64-alt-linux/13/ -B/usr/lib64/gcc/x86_64-alt-linux/13/")
set(CMAKE_SHARED_LINKER_FLAGS "-L/usr/lib64/gcc/x86_64-alt-linux/13/ -B/usr/lib64/gcc/x86_64-alt-linux/13/")
