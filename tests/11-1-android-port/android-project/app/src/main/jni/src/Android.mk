LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

# Add your application source files here...
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../../om/engine.cxx \
                   $(LOCAL_PATH)/../../../../../../om/imgui.cpp \
                   $(LOCAL_PATH)/../../../../../../om/imgui_demo.cpp \
                   $(LOCAL_PATH)/../../../../../../om/imgui_draw.cpp \
                   $(LOCAL_PATH)/../../../../../../om/imgui_impl_sdl_gl3.cpp

LOCAL_SHARED_LIBRARIES := SDL2
LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := game
LOCAL_SHARED_LIBRARIES := main
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../../../../
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../../game/game.cxx
include $(BUILD_SHARED_LIBRARY)