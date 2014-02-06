LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#include ./includeOpenCV.mk
#ifeq ("$(wildcard $(OPENCV_MK_PATH))","")
    #try to load OpenCV.mk from default install location
    #include $(TOOLCHAIN_PREBUILT_ROOT)/user/share/OpenCV/OpenCV.mk
    #include ../../OpenCV-2.3.1/OpenCV-2.3.1/share/OpenCV/OpenCV.mk
#else
    #include $(OPENCV_MK_PATH)
#endif

OPENCV_INSTALL_MODULES:=on

OPENCV_CAMERA_MODULES:=off

OPENCV_LIB_TYPE:=STATIC

include ../../../OpenCV-2.4.8-android-sdk/sdk/native/jni/OpenCV.mk

LOCAL_MODULE    := native_sample
LOCAL_SRC_FILES := jni_part.cpp
LOCAL_LDLIBS +=  -llog -ldl
APP_CFLAGS := -g

include $(BUILD_SHARED_LIBRARY)
