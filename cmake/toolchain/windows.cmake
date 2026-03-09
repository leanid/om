# Windows toolchain for prebuilt dependencies
# Use with: cmake -DCMAKE_TOOLCHAIN_FILE=.../windows.cmake ...
# Recommended: clang-cl from LLVM (https://releases.llvm.org/) for consistency with Linux build.
# MSVC from Visual Studio also works.

# Use lib (not lib64) for install dir on Windows
set(CMAKE_INSTALL_LIBDIR "lib" CACHE STRING "Installation directory for libraries")
# Если вы хотите продолжать использовать clang++,
# вам нужно явно сказать компилятору, что целью является MSVC ABI, а не GNU.
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --target=x86_64-pc-windows-msvc -nobuiltininc -nostdinc++" CACHE STRING "point clang++ to msvc abi")
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --target=x86_64-pc-windows-msvc -nobuiltininc" CACHE STRING "point clang++ to msvc abi")
