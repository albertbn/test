
LOCAL_PATH := $(call my-dir)

# libpng
LIBPNG_LOCAL := $(LOCAL_PATH)/libpng
LIBPNG_PATH := $(LOCAL_PATH)

include $(CLEAR_VARS)
LOCAL_MODULE:= libpngt
LOCAL_SRC_FILES := $(HOME)/dev/tess/tess-two/libs/$(TARGET_ARCH_ABI)/libpngt.so
LOCAL_EXPORT_C_INCLUDES := \
  $(LIBPNG_LOCAL) \
  $(LIBPNG_PATH)
LOCAL_SHARED_LIBRARIES := libz

include $(PREBUILT_SHARED_LIBRARY)

#leptonica
LEPTONICA_LOCAL := $(LOCAL_PATH)/com_googlecode_leptonica_android
LEPTONICA_PATH := $(LEPTONICA_LOCAL)/src

include $(CLEAR_VARS)

LOCAL_MODULE := liblept
LOCAL_SRC_FILES := $(HOME)/dev/tess/tess-two/libs/$(TARGET_ARCH_ABI)/liblept.so
LOCAL_EXPORT_C_INCLUDES := \
  $(LEPTONICA_LOCAL) \
  $(LEPTONICA_PATH)/src \
  $(LIBPNG_PATH)
LOCAL_SHARED_LIBRARIES:= libpngt

include $(PREBUILT_SHARED_LIBRARY)

#tesseract
TESSERACT_LOCAL := $(LOCAL_PATH)/com_googlecode_tesseract_android
TESSERACT_PATH := $(TESSERACT_LOCAL)/src

include $(CLEAR_VARS)

LOCAL_MODULE := libtess
LOCAL_SRC_FILES := $(HOME)/dev/tess/tess-two/libs/$(TARGET_ARCH_ABI)/libtess.so
LOCAL_EXPORT_C_INCLUDES := \
  $(LOCAL_PATH) \
  $(TESSERACT_PATH)/api \
  $(TESSERACT_PATH)/ccmain \
  $(TESSERACT_PATH)/ccstruct \
  $(TESSERACT_PATH)/ccutil \
  $(TESSERACT_PATH)/classify \
  $(TESSERACT_PATH)/cube \
  $(TESSERACT_PATH)/cutil \
  $(TESSERACT_PATH)/dict \
  $(TESSERACT_PATH)/opencl \
  $(TESSERACT_PATH)/neural_networks/runtime \
  $(TESSERACT_PATH)/textord \
  $(TESSERACT_PATH)/viewer \
  $(TESSERACT_PATH)/wordrec \
  $(LEPTONICA_PATH)/src \
  $(TESSERACT_LOCAL)
LOCAL_SHARED_LIBRARIES := liblept

include $(PREBUILT_SHARED_LIBRARY)

#opencv
include $(CLEAR_VARS)
OPENCV_LIB_TYPE:=STATIC
OPENCV_PACKAGE_DIR:= ~/dev/OpenCV-android-sdk/sdk

# 18 feb 2016 - shall we yep - remarked the := off for camera modules and set the modules for native to true. yep? i hope we yep :)
# OPENCV_CAMERA_MODULES := off
OPENCV_CAMERA_MODULES:=on
OPENCV_INSTALL_MODULES:=on
include $(OPENCV_PACKAGE_DIR)/native/jni/OpenCV.mk

LOCAL_SRC_FILES  := jni_java_native_bridge.cpp
LOCAL_SRC_FILES  += converters.cpp
LOCAL_SRC_FILES  += ofstream_child.cpp
LOCAL_SRC_FILES  += multipleObjectTracking.cpp
LOCAL_SRC_FILES  += ocr/db_scan.cc
LOCAL_SRC_FILES  += ocr/main.cc
LOCAL_SRC_FILES  += ocr/tess.cc

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_MODULE     := jni_java_native_bridge

# -lz from libpngt, -ljnigraphics from libtess
LOCAL_LDLIBS    += -llog -ldl -lz -ljnigraphics
LOCAL_SHARED_LIBRARIES += libtess liblept

include $(BUILD_SHARED_LIBRARY)
