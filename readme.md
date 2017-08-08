# OM Project

[![Om](https://bitbucket.org/account/user/b_y/projects/OM/avatar/32)](https://bitbucket.org/account/user/b_y/projects/OM)

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

```sh
$ git clone git@bitbucket.org:b_y/om.git
$ sudo apt install libsdl2-dev
$ cmake -G"Makefiles" 
$ make -j 4
$ bin/engine --test
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

