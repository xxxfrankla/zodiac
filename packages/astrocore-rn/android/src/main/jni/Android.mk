LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := astrocore
LOCAL_SRC_FILES := \
  ../../../../cpp/src/time.cpp \
  ../../../../cpp/src/transform.cpp \
  ../../../../cpp/src/vector.cpp \
  ../../../../cpp/src/engine.cpp \
  ../../../../cpp/src/jsi_bindings.cpp \
  ../../../../cpp/src/AstroCoreHostObject.cpp

LOCAL_CFLAGS := -std=c++20 -O2 -DNDEBUG
LOCAL_LDLIBS := -llog

LOCAL_C_INCLUDES := \
  $(LOCAL_PATH)/../../../../cpp/include

include $(BUILD_SHARED_LIBRARY)
