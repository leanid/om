# how to build linux
1. install GLM library on Fedora: sudo dnf install glm-devel
2. include #include <glm/glm.hpp>

# how to build android

1. install android sdk
2. install android ndk
3. download sources of SDL3
4. uncompress it to some place "~/SDL3"
5. create directory            "~/SDL3_build"
6. create local.properties in android-gradle directory example:
```
ndk.dir=/home/leo/Android/Sdk/ndk-bundle
sdk.dir=/home/leo/Android/Sdk
# If you set this property, Gradle no longer uses PATH to find CMake.
#cmake.dir=/home/leo/tools/cmake-3.12.0-Linux-x86_64
cmake.dir=C\:\\Program Files\\CMake
sdl3_src.dir=/home/leo/SDL3
sdl3_build.dir=/home/leo/SDL3_build
```
7. if you want to build arm64v of x86_64 you have to set minSdkVersion 18 - android 4.4
8. add to your build.gradle
```
	    externalNativeBuild {
	        cmake {
                Properties properties = new Properties()
                properties.load(project.rootProject.file('local.properties').newDataInputStream())
                def sdl3SrcDir = properties.getProperty('sdl3_src.dir') ?: "add sdl3_src.dir in local.properties"
                def sdl3BuildDir = properties.getProperty('sdl3_build.dir') ?: "add sdl3_build.dir in local.properties"

                arguments "-DSDL3_SRC_DIR=" + sdl3SrcDir
                arguments "-DSDL3_BUILD_DIR=" + sdl3BuildDir
                arguments "-DANDROID_ARM_NEON=TRUE"
	            arguments "-DANDROID_CPP_FEATURES=rtti exceptions",
	                      "-DANDROID_STL=c++_shared"
	        }
```

9. To enable OpenGL ES 3.0 support for android emulator your hardware should support OpenGL 4.2
or better. You can also force enable GLES3 in this case by issuing
`echo "GLESDynamicVersion = on" >> ~/.android/advancedFeatures.ini`
