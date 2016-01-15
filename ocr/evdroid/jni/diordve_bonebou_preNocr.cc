
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

#include <iostream>
// #include <stdlib.h>
// #include <fstream>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

// REMARK for mac debug
#include <allheaders.h>
#include <baseapi.h>
#include "strngs.h"
#include "genericvector.h"

// UNMARK for mac debug
// #include <leptonica/allheaders.h>
// #include <tesseract/baseapi.h>
// #include <tesseract/strngs.h>
// #include <tesseract/genericvector.h>

//====bonebou_includes===========
#include "diordve_bonebou_preNocr.h"
#include "all_headers.hpp"
#include "util/main_geometry.hpp"
//====end_bonebou_includes=======

using namespace cv;
using namespace std;

std::vector<cv::Vec4i> lines4intersect;
std::vector<bool> lines4intersect_is_vert;
std::vector<Point> p_from_line_vector;
cv::Point center(0,0);
Size size_mat;
double MIN_LINE_LENGTH_CONSIDERED_SIDE;
const char* tessdata_path_pref;

// go on from here
// http://answers.opencv.org/question/14881/hidden-symbol-__aeabi_atexit-in-is-referenced-by-dso/

JNIEXPORT void JNICALL Java_diordve_bonebou_preNocr_doit (
    JNIEnv *env, jobject thisObj, jstring path_sd_card, jstring img_path ) {

  tessdata_path_pref = (*env).GetStringUTFChars(path_sd_card, 0);
  char tessdata_path_pref_str[128], dump[128];
  strcpy(dump, tessdata_path_pref);  strcat(dump,"/tessdata/dump.txt");
  // PRE READY IMG
  strcpy ( tessdata_path_pref_str, tessdata_path_pref );
  // strcat ( tessdata_path_pref_str,"/tessdata/long8.jpg" ); /*long8.png is the whole invoice*/

  // const char* inputfile = tessdata_path_pref_str;

  Mat mat = imread ( get_path_absolute("/tessdata/long8.jpg") ); /*yep!*/

  longest_closed ( mat /*referral variable */ );

  (*env).ReleaseStringUTFChars(path_sd_card, tessdata_path_pref);
  // (*env).ReleaseStringUTFChars(img_path);
}


