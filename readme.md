# OM Project

[![Om](https://bitbucket.org/account/user/b_y/projects/OM/avatar/32)](https://bitbucket.org/account/user/b_y/projects/OM)

| Build Platform | Status (tests only)                                                              |
|----------------|----------------------------------------------------------------------------------|
| MSVC 2017 x64  | ![MSVC 2017 Build](https://ci.appveyor.com/api/projects/status/bitbucket/b_y/om) |
| Linux x64      | ![Linux x64](https://img.shields.io/bitbucket/pipelines/b_y/om.svg)              |

Om is a nice 2d game engine for multiplatform studying.

- Environmental friendliness (take less do more with c++)
- Modern IT technology (**c++20**, **cmake-3.26**, **gradle**)
- Creativity (create funny tools with **imgui** library)
- Fun 🤣

## Planed Features

- multiplatform (Linux, windows, Mac OS, Android, iOS)
- 2d on top of OpenGL 2.0 ES (3.0 ES)
- last c++20(c++23) standard
- STL enabled, no custom monkey coding
- easy building with modern cmake

### Tech

Om project uses a number of open source projects to work properly:

- [SDL3] - best cross-platform low level game library
- Om Project is open source with a public repository
- [om](https://bitbucket.org/b_y/om) on BitBucket.

### Installation

Om requires [SDL3](http://libsdl.org/) v3.0.0+ to run.

Install the dependencies to build engine on Linux.

#### On Ubuntu 

```sh
git clone git@bitbucket.org:b_y/om.git
sudo apt install libsdl2-dev
cmake -G"Makefiles"
make -j 4
bin/engine --test
```

#### On Fedora

```sh
git clone git@bitbucket.org:b_y/om.git
sudo dnf install SDL3
sudo dnf install SDL3-static
cmake -G"Makefiles"
make -j 4
bin/engine --test
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
    sdl2-config --version
    sdl2-config --libs
    sdl2-config --static-libs
    ```

### On Mac OS (with gcc)

#### using g++ from Homebrew (need c++17 support)

On macOS for c++17 compiler you have to install latest gcc from
Homebrew and then use it

```sh
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
brew install gcc
brew install SDL3
brew install cmake
```

### On Windows

#### for Visual Studio (2022 with c++23 support)

1. install **vcpkg** from [vcpkg](https://github.com/Microsoft/vcpkg)
2. install **SDL3** in **vcpkg**: `vcpkg install sdl3`
3. make directory build in **om/tests**. move into it and there:
   `cmake .. -DCMAKE_TOOLCHAIN_FILE={YOUR_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake`
4. build all using:

   ```cmd
   "C:\Program Files (x86)\Microsoft Visual Studio\ \
   2017\Community\MSBuild\15.0\Bin\MSBuild.exe" all-tests-build.sln
   ```

### Building

#### On Windows using [MSYS2](https://www.msys2.org)

1. from **msys2** bash shell:
    `pacman -S base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-SDL3`
2. (optional) full build development environment on windows using msys2:
    `pacman -S base-devel mingw-w64-x86_64-toolchain \
    git mingw-w64-x86_64-cmake mingw-w64-x86_64-qt-creator ninja`
3. from **cmd.exe** Windows terminal program (g++ in PATH from msys2)

    ```
    cd ~/om
    mkdir build
    cd build
    cmake ../tests
    cmake --build .
    ```

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

### Tools

#### Simple configuration for **doomemacs** per-project c++ cmake

1. create in project root file: **.dir-locals.el**
2. place next code into it.
3. edit it if you need add some cmake flags or change test-command

    ```elisp
    ;;; Directory Local Variables
    ;;; For more information see (info "(emacs) Directory Variables")

    ((nil . ((projectile--cmake-manual-command-alist . ((:configure-command . "cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S . -B build")
                                                        (:compile-command . "cmake --build build")
                                                        (:test-command . "./build/30-1-point-shadow"))))))
    ```

4. also you may want to disable warning about every opened file in project.
   Place next into: **~/.doom.d/config.el** or **~/.config/doom/config.el**

    ```elisp
    (put 'projectile--cmake-manual-command-alist 'safe-local-variable (lambda (_) t))
    ```

5. one more hint. Place **.projectile** file in root of your
   project to force doomemacs use this directory as root

#### Want latest and fastest Emacs?

1. on fedora install build dependencies to build Emacs from source:

    ```sh
    sudo dnf install gtk+-devel libXaw-devel libjpeg-devel libpng-devel \
    giflib-devel libtiff-devel gnutls-devel ncurses-devel Xaw3d-devel \
    libgccjit-devel harfbuzz-devel jansson-devel cairo-devel \
    ripgrep fd-find libtool
    ```

2. build Emacs with native-compilation enabled

    ```sh
    # configure emacs for native-compilation like next:
    ../emacs-28.1/configure --with-native-compilation --with-mailutils
    ```

3. after installing just fresh Emacs call sync
   to regenerate-compile elisp packages for doomemacs

    ```sh
    ~/.doom.d/bin/doom sync
    ```

### License

**ZIP**

[//]: # (These are reference links used in the body of this note and get stripped out when the markdown processor does its job. There is no need to format nicely because it shouldn't be seen. Thanks SO - http://stackoverflow.com/questions/4823468/store-comments-in-markdown-syntax)
[SDL3]: <http://libsdl.org/>

