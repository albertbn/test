
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
#include "diordve_bonebou_preNocr.h"

// REMARK for mac debug
#include <android/log.h>
#define LOG_TAG "evdroid"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))

#include <stdlib.h>
#include <iostream>
#include <fstream>

#include <opencv2/core/core.hpp>

// REMARK for mac debug
// #include <allheaders.h>
// #include <baseapi.h>
// #include "strngs.h"
// #include "genericvector.h"

// UNMARK for mac debug
#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>
#include <tesseract/strngs.h>
#include <tesseract/genericvector.h>

using namespace std;

JNIEXPORT void JNICALL Java_diordve_bonebou_preNocr_doit (
                                                          JNIEnv *env, jobject thisObj, jstring jtessdata_path_pref, jstring jpng_path ) {
// int main ( ) {
  char tessdata_path_pref_str[128], dump[128];
  const char* tessdata_path_pref = (*env).GetStringUTFChars(jtessdata_path_pref, 0);
                                                         ;
  strcpy(dump, tessdata_path_pref);  strcat(dump,"/tessdata/dump.txt");

  // PRE READY IMG
  strcpy(tessdata_path_pref_str, tessdata_path_pref);  strcat(tessdata_path_pref_str,"/tessdata/long8_part.png"); /*long8.png is the whole invoice*/
  const char* inputfile = tessdata_path_pref_str;

 // REAL IMG
  // const char* inputfile = (*env).GetStringUTFChars(jpng_path, 0);

  char *outText = NULL;
  tesseract::Orientation orientation;
  tesseract::WritingDirection direction;
  tesseract::TextlineOrder order;
  float deskew_angle;

  tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
  LOGD("Using tesseract c++ API: %s ", api->Version());

  PIX* image = pixRead ( inputfile );
  LOGD ( "Lept Image path: %s\n", inputfile );
  LOGD ( "Lept Image size (w,h) : %d, %d\n", image->w, image->h );
  // return;

  GenericVector<STRING> vars_vec;
  vars_vec.push_back("load_system_dawg");

  GenericVector<STRING> vars_values;
  vars_values.push_back("F");

  api->Init ( tessdata_path_pref, "heb", tesseract::OEM_DEFAULT , NULL, 0, &vars_vec, &vars_values, false );

  api->SetVariable("language_model_penalty_non_dict_word", "0");

  ofstream outfile;
  outfile.open ( dump, ios_base::app );
  api->SetPageSegMode(tesseract::PSM_AUTO);

  int len = strlen(tessdata_path_pref_str); len-=10;
  for ( int i=0; i<5; ++i ) {

    tessdata_path_pref_str[len] = i+48;
    inputfile = tessdata_path_pref_str;
    // outfile << inputfile;
    image = pixRead ( inputfile );
    // UNMARK for OSD
    // api->SetPageSegMode(tesseract::PSM_AUTO_OSD);
    api->SetImage(image);
    api->Recognize(0);

    // ========= START UNMARK for osd =============
    // tesseract::PageIterator* it =  api->AnalyseLayout();
    // it->Orientation(&orientation, &direction, &order, &deskew_angle);

    // LOGD ( "Orientation: %d;\nWritingDirection: %d\nTextlineOrder: %d\n"    \
    //        "Deskew angle: %.4f\n",
    //        orientation, direction, order, deskew_angle );

    // if ( orientation == 3 ) {
    // // if ( 1==1 ) {

    //   image = pixRotate90( image, 1 );
    //   // printf("ok, orientations is 3\n=======\n");
    //   LOGD("ok, orientations is 3\n=======\n");
    //   api->SetImage(image);
    //   api->Recognize(0);

    //   // it =  api->AnalyseLayout();
    //   // it->Orientation(&orientation, &direction, &order, &deskew_angle);
    // }

    // api->SetPageSegMode ( tesseract::PSM_AUTO );
    // ========= END UNMARK for osd =============

    // outText = api->GetUTF8Text(NULL /*what the fock???*/);
    outText = api->GetUTF8Text();
    LOGD ( "OCR output: %s", outText );

    outfile << outText;
  }
  outfile.close();

  // Destroy used object and release memory
  api->End();
  delete [] outText;
  pixDestroy(&image);

  (*env).ReleaseStringUTFChars(jtessdata_path_pref, tessdata_path_pref);
  (*env).ReleaseStringUTFChars(jpng_path, inputfile);
}
