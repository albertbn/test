
// from jni
// g++ -I"$JAVA_HOME/include" -I"$JAVA_HOME/include/darwin" -shared  -o preNocr.so diordve_bonebou_preNocr.cc -llept -ltesseract

// from src
// javac diordve/bonebou/preNocr.java && java diordve/bonebou/preNocr

// g++ -o preNocr preNocr.cc -llept -ltesseract && ./preNocr

// javac preNocr.java && java preNocr

// tess source
// https://fossies.org/dox/tesseract-ocr-3.02.02/baseapi_8cpp_source.html#l00213
// http://stackoverflow.com/questions/2483978/best-way-to-implement-globally-scoped-data - c++ extern and static
// http://p2p.wrox.com/c-programming/92954-c-class-example-separate-header-implementation-file.html - c++ class header and implement

// TEMP
#include <jni.h>

#include <iostream>
#include <fstream>
#include <ctime>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

//====bonebou_includes===========
#include "diordve_bonebou_preNocr.h"
#include "all_headers.hpp"
#include "util/main_geometry.hpp"
//====end_bonebou_includes=======

using namespace cv;
using namespace std;

unsigned int clock_start;
ofstream outfile;
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

#ifdef ANDROID
JNIEXPORT void JNICALL Java_diordve_bonebou_preNocr_doit (
    JNIEnv *env, jobject thisObj, jstring jpath_sd_card, jstring jimg_path ) {

  unsigned int clock_start_main = clock();
  clock_start = clock();

  remove ( (path_sd_card + "/tessdata/dump.txt").c_str() );
  outfile.open ( (path_sd_card + "/tessdata/dump.txt").c_str(), ios_base::app );

  path_sd_card = (*env).GetStringUTFChars(jpath_sd_card, 0);
  path_img = path_sd_card + "/tessdata/img";

  LOGD ( "path: %s \n", (path_sd_card + "/tessdata/heb.jpg").c_str() );

  // TEMP test
  Mat mat = imread ( path_sd_card + "/tessdata/heb.jpg" ); /*yep!*/
  // string img_path = (*env).GetStringUTFChars(jimg_path, 0);
  // Mat mat = imread ( img_path ); /*yep!*/

  longest_closed ( mat /*referral variable */ );

  mat.release();

  outfile << "total time: " << clock() - clock_start_main ;
  outfile.close();
  // (*env).ReleaseStringUTFChars(jpath_sd_card, path_sd_card.c_str());
}
#endif // ANDROID

int main ( int argc, char** argv )
{
  if( argc < 2  ){
    cout << "please pass an image" << endl;
    return 1;
  }

  unsigned int clock_start_main = clock();
  clock_start = clock();

  remove( (path_img + "/dump.txt").c_str() );
  outfile.open ( (path_img + "/dump.txt").c_str(), ios_base::app ); /*regular exe computer*/

  cout << "evdroid processing img.. " << argv[1] << endl;

  path_sd_card.clear();
  path_img = "./img";

  Mat mat = imread ( argv[1] ); /*yep!*/

  longest_closed ( mat /*referral variable */ );

  mat.release();

  outfile << "total time: " << clock() - clock_start_main ;
  outfile.close();
  return 0;
}
