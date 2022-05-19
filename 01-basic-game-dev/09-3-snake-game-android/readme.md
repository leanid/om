## 1. First make shure your local.properties file in good shape
### Example:
```sh
#ndk.dir=/home/leo/Downloads/android-ndk-r18b-linux-x86_64/android-ndk-r18b
ndk.dir=/home/leo/Android/Sdk/ndk-bundle
sdk.dir=/home/leo/Android/Sdk
sdl2_src.dir=/home/leo/Downloads/SDL2-2.0.10
sdl2_build.dir=/home/leo/Downloads/SDL2-2.0.10-build
# if you want to use your default system cmake pass prefix to bin/cmake
#cmake.dir=/usr
# if you want to use exact cmake download it from oficial site
#cmake.dir=/home/leo/Download/cmake-3.17.3-Linux-x86_64
cmake.dir=/usr
```

## 2. Double check path to cmake see comments in previous example
## 3. Check path to SDL2 source directory
## 4. Add binary dir for every build configuration as we use out of build tree path to SDL2: When specifying an out-of-tree source a binary directory must be explicitly specified.
