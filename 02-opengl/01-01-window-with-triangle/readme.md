# how to build android

1. install android sdk
2. install android ndk
3. download sources of SDL2
4. uncompress it to some plase "~/SDL2-2.0.8"
5. create directory            "~/SDL2-2.0.0_build"
6. create local.properties in android-gradle directory example:
```
ndk.dir=/home/leo/Android/Sdk/ndk-bundle
sdk.dir=/home/leo/Android/Sdk
# If you set this property, Gradle no longer uses PATH to find CMake.
#cmake.dir=/home/leo/tools/cmake-3.12.0-Linux-x86_64
cmake.dir=C\:\\Program Files\\CMake
sdl2_src.dir=/home/leo/SDL2-2.0.8
sdl2_build.dir=/home/leo/SDL2-2.0.8_build
```
7. if you want to build arm64v of x86_64 you have to set minSdkVersion 18 - android 4.4
8. add to your build.gradle
```
	    externalNativeBuild {
	        cmake {
                Properties properties = new Properties()
                properties.load(project.rootProject.file('local.properties').newDataInputStream())
                def sdl2SrcDir = properties.getProperty('sdl2_src.dir') ?: "add sdl2_src.dir in local.properties"
                def sdl2BuildDir = properties.getProperty('sdl2_build.dir') ?: "add sdl2_build.dir in local.properties"

                arguments "-DSDL2_SRC_DIR=" + sdl2SrcDir
                arguments "-DSDL2_BUILD_DIR=" + sdl2BuildDir
                arguments "-DANDROID_ARM_NEON=TRUE"
	            arguments "-DANDROID_CPP_FEATURES=rtti exceptions",
	                      "-DANDROID_STL=c++_shared"
	        }
```

9. To enable OpenGL ES 3.0 support for android emulator your hardware should support OpenGL 4.2
or better. You can also force enable GLES3 in this case by issuing
`echo "GLESDynamicVersion = on" >> ~/.android/advancedFeatures.ini`
