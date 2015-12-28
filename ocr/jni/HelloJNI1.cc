
// javac HelloJNI.java && java HelloJNI

// g++ -I"$JAVA_HOME/include" -I"$JAVA_HOME/include/darwin" -shared  -o hello.so HelloJNI.cc -llept -ltesseract
// g++ -o HelloJNI HelloJNI.cc -llept -ltesseract && ./HelloJNI

// tess source
// https://fossies.org/dox/tesseract-ocr-3.02.02/baseapi_8cpp_source.html#l00213

// TEMP
#include <jni.h>
#include "HelloJNI.h"

#include <iostream>
#include <stdlib.h>
#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>
#include "tesseract/strngs.h"
#include "tesseract/genericvector.h"

using namespace std;

tesseract::TessBaseAPI tess;
tesseract::Orientation orientation;
tesseract::WritingDirection direction;
tesseract::TextlineOrder order;
float deskew_angle;

void init_ocr ( ) {

  GenericVector<STRING> vars_vec;
  vars_vec.push_back("load_system_dawg");

  GenericVector<STRING> vars_values;
  vars_values.push_back("F");

  tess.Init("/usr/local/Cellar/tesseract/3.02.02/", "heb", tesseract::OEM_DEFAULT);

  tess.SetPageSegMode(tesseract::PSM_AUTO_OSD);
}

JNIEXPORT void JNICALL Java_HelloJNI_sayHello ( JNIEnv *env, jobject thisObj ) {
// int main ( ) {

  init_ocr();
}
