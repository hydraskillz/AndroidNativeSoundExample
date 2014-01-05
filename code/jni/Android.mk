LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_LDLIBS    := -llog -landroid
LOCAL_MODULE    := NativeSoundExample
LOCAL_SRC_FILES := NativeSoundExample.cpp
LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)