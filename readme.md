# OM Project

[![Om](https://bitbucket.org/account/user/b_y/projects/OM/avatar/32)](https://bitbucket.org/account/user/b_y/projects/OM)


Build Platform   | Status (tests only)
---------------- | ----------------------
MSVC 2017 x64    | ![MSVC 2017 Build](https://ci.appveyor.com/api/projects/status/bitbucket/b_y/om)
Linux x64        | ![Linux x64](https://img.shields.io/bitbucket/pipelines/b_y/om.svg)



Om is a nice 2d game engine for multy platform using. Use it and develop it if you like next principals:

  - Environmental friendliness (take less do more with c++)
  - Modern IT technology (c++17, cmake-3.14, gradle)
  - Creativity (create funny tools with imgui library)

# Planed Features!

  - multi platform (linux, windows, mac os, android, ios)
  - 2d on top of OpenGL 2.0 ES
  - last c++17 standard
  - STL enabled, no custom monkey coding
  - easy building with cmake

### Tech

Om project uses a number of open source projects to work properly:

* [SDL2] - best crossplatform game library

 Om Project is open source with a public repository [om](https://bitbucket.org/b_y/om)
 on BitBucket.

### Installation

Om requires [SDL2](http://libsdl.org/) v2.0.9+ to run.

Install the dependencies to build engine on linux.

#### On Ununtu linux

```sh
$ git clone git@bitbucket.org:b_y/om.git
$ sudo apt install libsdl2-dev
$ cmake -G"Makefiles" 
$ make -j 4
$ bin/engine --test
```

#### On Fedora linux
```sh
$ git clone git@bitbucket.org:b_y/om.git
$ sudo dnf install SDL2
$ sudo dnf install SDL2-static
$ cmake -G"Makefiles" 
$ make -j 4
$ bin/engine --test
```
##### hints on Fedora
1. To search package
```sh
# search by package name
$ dnf search SDL2
# search by file in package
$ dnf provides /usr/lib/libSDL2.so
```
2. To list package contents (files list)
```sh
$ dnf repoquery -l SDL2
$ rpm -ql SDL2
```
3. To show compile/link/version of installed version
```sh
$ sdl2-config --version
$ sdl2-config --libs
$ sdl2-config --static-libs
```

### On Mac OS (with gcc)
#### using g++ from Homebrew (need c++17 support)
on Mac OS for c++17 compiler you have to install latest gcc from Homebrew and then use it
```sh
$ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
$ brew install gcc
$ brew install SDL2
$ brew install cmake
```

### On Windows
#### for Visual Studio (2017 with c++17 support)
1. install vcpkg from (https://github.com/Microsoft/vcpkg)
2. install SDL2 in vcpgk: ```vcpkg install sdl2```
3. make directory build in om/tests. move into it and there:
```cmake .. -DCMAKE_TOOLCHAIN_FILE={YOUR_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake```
4. build all using:
```"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\MSBuild.exe" all-tests-build.sln```

### Building
#### On Windows using MSYS2(https://www.msys2.org)
1. from msys2 bash shell:
    - pacman -S base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-SDL2
2. from cmd.exe windows terminal program (g++ in PATH from msys2)
    - cd ~/om 
    - mkdir build 
    - cd build 
    - cmake ../tests 
    - cmake --build .

### Generate Docker image (for bitbucket pipelines)
 - write Dockerfile
 - call ```sudo systemctl start docker```
 - call ```sudo docker build -t leanid/fedora30 .```
 - call ```sudo docker push leanid/fedora30```

Dockerfile content:

```sh
FROM fedora:30

RUN dnf update -y
RUN dnf upgrade -y
RUN dnf install -y gcc-c++ make cmake mingw64-gcc mingw64-gcc-c++ clang wine git SDL2-devel SDL2-static mingw64-SDL2 mingw64-SDL2-static
```

### Todos

 - write better readme.md about installation SDL on all platforms

License
----

**ZIP**

[//]: # (These are reference links used in the body of this note and get stripped out when the markdown processor does its job. There is no need to format nicely because it shouldn't be seen. Thanks SO - http://stackoverflow.com/questions/4823468/store-comments-in-markdown-syntax)


   [SDL2]: <http://libsdl.org/>

