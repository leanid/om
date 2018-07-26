
# Uncomment this if you're using STL in your project
# See CPLUSPLUS-SUPPORT.html in the NDK documentation for more information
APP_STL := c++_shared
APP_CPPFLAGS := -std=c++1z -fexceptions -frtti

# APP_ABI := armeabi armeabi-v7a x86
APP_ABI := x86 # build for emulator only

# Min SDK level
APP_PLATFORM=android-10

