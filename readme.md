# OM Project (🕉)

| Build Platform | Status (tests only)                                                              |
|----------------|----------------------------------------------------------------------------------|
| MSVC 2017 x64  | ![MSVC 2017 Build](https://ci.appveyor.com/api/projects/status/bitbucket/b_y/om) |
| Linux x64      | ![Linux x64](https://img.shields.io/bitbucket/pipelines/b_y/om.svg)              |

Om is a collection of several mini tutorial about:

- c++, gamedev, linux, crossplatform
- Modern c++ IT technology (**c++23**, **cmake-4.0+**, **gradle**)
- Fun 🤣

## Planed Features

- basic shell (bash, zsh) example
- basic c++ (terminal, strings, memory, error handling)
- basic gamedev (input, render2d - vulkan/gles, sound)
- multiplatform (Linux, windows, Mac OS, Android, iOS)
- 2d on top of OpenGL 2.0 ES (3.0 ES)
- 2d on top of Vulkan 1.3
- easy building with modern cmake

### Tech

Om project uses a number of open source projects to work properly:

- [SDL3](https://github.com/libsdl-org/SDL) - best cross-platform low level game library
- [Boost](https://www.boost.org/) - best c++ system level libraries
- [GLM](https://github.com/g-truc/glm) - best c++ math library for gamedev

### Installation

Om requires [Cmake](https://cmake.org/) v4.0+ to build.

Install the dependencies to build tutorials.

#### On Alt Linux (Regular over p11)
```sh
# for llvm with libc++
sudo epm install libcxx-devel
sudo epm install libcxx-devel libcxx-static
# needed for libstdc++exp.a
sudo epm install libstdc++14-devel-static
# for SDL3 (deps)
sudo epm install libwayland-client-devel libwayland-server-devel libEGL-devel
sudo epm install libGL-devel
sudo epm install libalsa-devel libjack-devel
sudo epm install libpulseaudio-devel
sudo epm install libsndio7-devel
sudo epm install libdrm-devel
sudo epm install libgbm-devel
sudo epm install libwayland-egl-devel
sudo epm install libwayland-cursor-devel
sudo epm install libxkbcommon-devel
sudo epm install libunwind-devel
sudo epm install libusb-devel
sudo epm install libudev-devel
sudo epm install libdecor-devel
# for boost (deps)
sudo epm install libffi-devel
sudo epm install libssl-devel
# for Vulkan
sudo epm install libvulkan-devel
sudo epm install glslang
sudo epm install glslang-devel
sudo epm install glslc
# to install slangc - build it with VULKAN_SDK [./vulkansdk slangc -j 8]
# and add to .zshrc - source path_to_vk_sdk/setup-env.sh
```

#### On Fedora

```sh
sudo dnf install SDL3
sudo dnf install SDL3-static
```
##### hints on Alt
1. To search package

```sh
#search by package name
epm search sdl3
# search by file name ending example: (++exp.a) in package
epm filesearch \\+\\+exp\\.a 
```
2. To list package content

```sh
epm filelist <package>
```

##### hints on Fedora

1. To search package

    ```sh
    # search by package name
    dnf search SDL3
    # search by file in package
    dnf provides /usr/lib/libSDL3.so
    # search by part of file name
    dnf provides "*/libSDL3.so"
    ```

2. To list package contents (files list)

    ```sh
    dnf repoquery -l SDL3
    rpm -ql SDL3
    ```

3. To show compile/link/version of installed version

    ```sh
    sdl3-config --version
    sdl3-config --libs
    sdl3-config --static-libs
    ```

### On Mac OS (with gcc)

#### using g++ from Homebrew (need c++23 support)

On Mac OS X for c++23 compiler you have to install latest llvm from
Homebrew and then use it

### On Windows

1. install Vusial Studio Community
2. or install Mingw64w

### Generate Docker image (for BitBucket pipelines)

- read complete example in ```support/docker{Dockerfile|readme.md}```
- write Dockerfile
- call ```sudo systemctl start docker```
- call ```sudo docker build -t leanid/fedora_latest .```
- call ```sudo docker push leanid/fedora_latest```

Dockerfile content:

```sh
FROM fedora:latest

RUN dnf update -y
RUN dnf upgrade -y
RUN dnf install -y gcc-c++ make cmake mingw64-gcc mingw64-gcc-c++ \
clang wine git SDL3-devel SDL3-static mingw64-SDL3 mingw64-SDL3-static \
libstdc++-static glibc-static ninja-build
```

### Building

On every platform you should do same things like:
```sh
cmake . --preset ninja-llvm
cmake --build --preset ninja-llvm --config Debug
```

### Tools

#### Doomemacs

All current development is done in Doomemacs.
How to install Doomemacs [install](https://github.com/doomemacs/doomemacs)
How to configure and use [usage](~/support/emacs/doomemacs.md)

### License

**ZIP**

