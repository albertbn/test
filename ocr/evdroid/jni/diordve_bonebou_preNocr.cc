
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
string path_sd_card; /* doesn't end with / */
string path_img; /* doesn't end with / */

// go on from here
// http://answers.opencv.org/question/14881/hidden-symbol-__aeabi_atexit-in-is-referenced-by-dso/

JNIEXPORT void JNICALL Java_diordve_bonebou_preNocr_doit (
    JNIEnv *env, jobject thisObj, jstring jpath_sd_card, jstring jimg_path ) {

  path_sd_card = (*env).GetStringUTFChars(jpath_sd_card, 0);
  path_img = path_sd_card + "/tessdata/img";

  // LOGD ( "path: %s \n", get_path_absolute("/tessdata/long8.jpg") );

  Mat mat = imread ( path_sd_card + "/tessdata/heb.jpg" ); /*yep!*/

  longest_closed ( mat /*referral variable */ );

  // (*env).ReleaseStringUTFChars(jpath_sd_card, path_sd_card.c_str());
}
