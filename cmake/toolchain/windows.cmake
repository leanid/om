# Windows toolchain for prebuilt dependencies
# Use with: cmake -DCMAKE_TOOLCHAIN_FILE=.../windows.cmake ...
# Recommended: clang-cl from LLVM (https://releases.llvm.org/) for consistency with Linux build.
# MSVC from Visual Studio also works.

# Use lib (not lib64) for install dir on Windows
set(CMAKE_INSTALL_LIBDIR "lib" CACHE STRING "Installation directory for libraries")
