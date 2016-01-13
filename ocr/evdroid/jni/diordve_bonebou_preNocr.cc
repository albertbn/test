

// from jni
// g++ -I"$JAVA_HOME/include" -I"$JAVA_HOME/include/darwin" -shared  -o preNocr.so diordve_bonebou_preNocr.cc -llept -ltesseract

// from src
// javac diordve/bonebou/preNocr.java && java diordve/bonebou/preNocr

// g++ -o preNocr preNocr.cc -llept -ltesseract && ./preNocr

// javac preNocr.java && java preNocr

// tess source
// https://fossies.org/dox/tesseract-ocr-3.02.02/baseapi_8cpp_source.html#l00213

// TEMP
#include <jni.h>

// REMARK for mac debug
#include <android/log.h>
#define LOG_TAG "evdroid"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))

#include <iostream>
// #include <stdlib.h>
// #include <fstream>

// #include <opencv2/opencv.hpp>
// #include <opencv2/core/core.hpp>

// REMARK for mac debug
// #include <allheaders.h>
// #include <baseapi.h>
// #include "strngs.h"
// #include "genericvector.h"

// UNMARK for mac debug
// #include <leptonica/allheaders.h>
// #include <tesseract/baseapi.h>
// #include <tesseract/strngs.h>
// #include <tesseract/genericvector.h>

//====bonebou_includes===========
#include "diordve_bonebou_preNocr.h"
#include "all_headers.hpp"
#include <sys/stat.h>
//====end_bonebou_includes=======

// using namespace cv;
using namespace std;

JNIEXPORT void JNICALL Java_diordve_bonebou_preNocr_doit (
    JNIEnv *env, jobject thisObj, jstring jtessdata_path_pref, jstring jpng_path ) {

  if ( file_exists("fuck u pu") ) {
    cout << "fuck u" << endl;
  }
  // Mat mat = imread( "fucku.tiff"); /*yep!*/
}


