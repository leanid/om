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
on Mac OS for c++17 compiler you have to install latest gcc from Homebrew and then use it
```sh
$ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
$ brew install gcc@7
$ brew install SDL2
$ brew install cmake
```

### Generate Docker image
 - write Dockerfile
 - call ```sudo systemctl start docker```
 - call ```sudo docker build -t leanid/fedora26 .```
 - call ```sudo docker push leanid/fedora26```

Dockerfile content:

```sh
FROM fedora:26

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

