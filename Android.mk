LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
  dirtycow.c \
  shared.c \
  farm.c

LOCAL_MODULE := farm
LOCAL_LDFLAGS   += -llog
LOCAL_CFLAGS    += -fPIE -DDEBUG
LOCAL_LDFLAGS   += -fPIE -pie
LOCAL_C_INCLUDES += $(ANDROID_NDK_HOME)/platforms/android-23/arch-arm/usr/include

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
  bridge.c \
  shared.c

LOCAL_MODULE := bridge
LOCAL_CFLAGS    += -fPIE -DDEBUG
LOCAL_LDFLAGS   += -fPIE -pie -llog
LOCAL_C_INCLUDES += $(ANDROID_NDK_HOME)/platforms/android-23/arch-arm/usr/include

include $(BUILD_EXECUTABLE)
