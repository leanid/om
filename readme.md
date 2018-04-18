# OM Project

[![Om](https://bitbucket.org/account/user/b_y/projects/OM/avatar/32)](https://bitbucket.org/account/user/b_y/projects/OM)

![status](https://ci.appveyor.com/api/projects/status/bitbucket/b_y/om)

Om is a nice 2d game engine for multy platform using. Use it and develop it if you like next principals:

  - Environmental friendliness
  - Modern IT technology
  - Creativity

# Planed Features!

  - multi platform (linux, windows, mac os, android, ios)
  - 2d on top of OpenGL 2.0 ES
  - last c++17 standard
  - STL enabled, no custom monkey coding
  - easy building with cmake

### Tech

Om project uses a number of open source projects to work properly:

* [SDL2] - best crossplatform game library

And of course Om Project itself is open source with a public repository [om](https://bitbucket.org/b_y/om)
 on BitBucket.

### Installation

Om requires [SDL2](http://libsdl.org/) v2.0.5+ to run.

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

### On Mac OS
#### using g++ from Homebrew (need c++17 support)
on Mac OS for c++17 compiler you have to install latest gcc from Homebrew and then use it
```sh
$ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
$ brew install gcc@7
$ brew install SDL2
$ brew install cmake
```

### On Windows
#### for MinGW 
1. install MinGW-64 from (http://mingw-w64.sourceforge.net/)
2. download SDL2 from (https://www.libsdl.org/release/SDL2-devel-2.0.8-mingw.tar.gz)
3. extract SDL2-devel-2.0.8-mingw.tar.gz into your MinGW-64 installation (lib -> lib, include -> include ...etc.)
4. install cmake from (https://cmake.org/)
5. ```cmake -G "MinGW Makefiles" om/tests/```
6. ```mingw32-make```

#### for Visual Studio (2017 with c++17 support)
1. install vcpkg from (https://github.com/Microsoft/vcpkg)
2. install SDL2 in vcpgk: ```vcpkg install sdl2```
3. make directory build in om/tests. move into it and there:
```cmake .. -DCMAKE_TOOLCHAIN_FILE={YOUR_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake```
4. build all using:
```"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\MSBuild.exe" all-tests-build.sln```

### Building
#### On Windows using MSYS2
1. install cmake from (https://cmake.org/download/), if you haven't done this before.
    - while installing, check the "add Cmake to the system PATH for all users" option.
    - make sure that you have ".../Cmake/bin" in your system PATH.
2. install git from (https://git-scm.com/download/win), if you haven't done this before.
    - while installing, check the "use git from the windows command prompt" option.
    - make sure that you have ".../Git/cmd" in your system PATH.
3. download MSYS2 from (https://www.msys2.org)
4. set-up and update MSYS2 following the instructions on web-page:
    - set-up in default folder (i.e. C:/msys64)
    - during update system may note that "msys2-runtime and catgets conflicts", feel free to remove catgets and libcatgets.
    - on some systems installation may hung up after terminate request. kill the process and restart update.
5. update is a MUST!
6. close all MSYS2 launchers.
7. add new environment variable names MSYS2_PATH_TYPE with value inherit.
8. start MSYS MinGw 64-bit from start menu, or C:/msys64/mingw64.exe
9. clone repo with "git clone https://bitbucket.org/b_y/om.git"
10-A. build the project with MSYS2 bash's script with "~/om/om-build.sh"
10-B. build the project manual:
    - pacman -S base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-SDL2
    - cd ~/om 
    - mkdir build 
    - cd build 
    - cmake ../tests -G "MSYS Makefiles" 
    - cmake --build .

### Generate Docker image (for bitbucket pipelines)
 - write Dockerfile
 - call ```sudo systemctl start docker```
 - call ```sudo docker build -t leanid/fedora26 .```
 - call ```sudo docker push leanid/fedora26```

Dockerfile content:

```sh
FROM fedora:27

RUN dnf update -y
RUN dnf upgrade -y
RUN dnf install -y gcc-c++
RUN dnf install -y make
RUN dnf install -y cmake
RUN dnf install -y mingw64-gcc
RUN dnf install -y mingw64-gcc-c++
RUN dnf install -y clang
RUN dnf install -y wine
RUN dnf install -y git
RUN dnf install -y SDL2-devel
RUN dnf install -y SDL2-static
RUN dnf install -y mingw64-SDL2
RUN dnf install -y mingw64-SDL2-static
```

### Todos

 - Write initial TDD template
 - build on linux and windows
 - write better readme.md about installation SDL on all platforms

License
----

**ZIP**

[//]: # (These are reference links used in the body of this note and get stripped out when the markdown processor does its job. There is no need to format nicely because it shouldn't be seen. Thanks SO - http://stackoverflow.com/questions/4823468/store-comments-in-markdown-syntax)


   [SDL2]: <http://libsdl.org/>

