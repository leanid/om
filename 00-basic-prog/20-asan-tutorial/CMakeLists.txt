cmake_minimum_required(VERSION 3.18...3.24)

project(20-asan-tutotial CXX)

add_executable(20-asan-tutorial main.cxx)
target_compile_options(20-asan-tutorial PRIVATE -fsanitize=address)
target_link_options(20-asan-tutorial PRIVATE -fsanitize=address)

# how to check binary with ASAN
# 1. build Debug version to see nice file lines in erros
# 2. on MacOS and Linux you can use ASAN_OPTIONS=detect_leaks=1
# 3. more info here: https://releases.llvm.org/14.0.0/tools/clang/docs/AddressSanitizer.html
# 4. on Windows you can use MSYS2 with CLANG64
#   4.1 install `pacboy -S clang:c cmake:c ninja:c`
#   4.2 check `clang++ --version && cmake --version && ninja --version`
