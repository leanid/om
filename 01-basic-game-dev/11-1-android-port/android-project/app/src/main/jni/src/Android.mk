LOCAL_PATH := $(call my-dir)

OM_REL := $(LOCAL_PATH)/../../../../../../om

include $(CLEAR_VARS)

LOCAL_MODULE := main

LOCAL_C_INCLUDES := $(OM_REL) $(OM_REL)/backends

# Add your application source files here...
LOCAL_SRC_FILES := $(OM_REL)/engine.cxx \
                   $(OM_REL)/imgui.cpp \
                   $(OM_REL)/imgui_demo.cpp \
                   $(OM_REL)/imgui_draw.cpp \
                   $(OM_REL)/imgui_tables.cpp \
                   $(OM_REL)/imgui_widgets.cpp \
                   $(OM_REL)/backends/imgui_impl_sdl2.cpp \
                   $(OM_REL)/backends/imgui_impl_opengl3.cpp

LOCAL_SHARED_LIBRARIES := SDL2
LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := game
LOCAL_SHARED_LIBRARIES := main
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../../../../
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../../../../game/game.cxx
include $(BUILD_SHARED_LIBRARY)
