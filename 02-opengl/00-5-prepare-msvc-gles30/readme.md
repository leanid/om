How to build minimal OpenGL ES 3.0 under Windows 10 and Visual Studio 2017
1. install Visual Studio 2017 Comunity
2. install vcpkg from https://github.com/Microsoft/vcpkg
3. install vcpkg SDL2
4. mkdir build
5. cd build
6. cmake -DCMAKE_TOOLCHAIN_FILE=[path to vcpkg toolchain] ..
7. cmake --build .
8. if you want to disable console winodow just change linker options: subsystem from concole to windows
