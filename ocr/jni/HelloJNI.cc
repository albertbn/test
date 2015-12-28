
// g++ -I"$JAVA_HOME/include" -I"$JAVA_HOME/include/darwin" -shared  -o hello.so HelloJNI.cc -llept -ltesseract
// g++ -o HelloJNI HelloJNI.cc -llept -ltesseract && ./HelloJNI
// javac HelloJNI.java && java HelloJNI

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

JNIEXPORT void JNICALL Java_HelloJNI_sayHello ( JNIEnv *env, jobject thisObj ) {
// int main ( ) {

  setenv ( "TESSDATA_PREFIX","/usr/local/Cellar/tesseract/3.02.02/share/",1 );
  cout << getenv("TESSDATA_PREFIX") << endl;

  // printf("\n\nFuck the blacks\n\n");
  cout << "Hello World from C++!" << endl;
  /* return; */

  const char* inputfile = "../img_pre/long8.jpg";

  char *outText = NULL;
  tesseract::Orientation orientation;
  tesseract::WritingDirection direction;
  tesseract::TextlineOrder order;
  float deskew_angle;

  PIX *image = pixRead(inputfile);
  tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
  printf("Using tesseract c++ API: %s\n", api->Version());

  GenericVector<STRING> vars_vec;
  vars_vec.push_back("load_system_dawg");

  GenericVector<STRING> vars_values;
  vars_values.push_back("F");


  // api->Init("/usr/local/share/", "eng", tesseract::OEM_DEFAULT , NULL, 0, &vars_vec, &vars_values, false);
  // api->Init("/usr/local/Cellar/tesseract/3.02.02/share/", "heb", tesseract::OEM_DEFAULT , NULL, 0, &vars_vec, &vars_values, false);
  api->Init("/usr/local/Cellar/tesseract/3.02.02/share/", "heb", tesseract::OEM_DEFAULT , NULL, 0, &vars_vec, &vars_values, false);
  // cout << api->GetDatapath() << endl;

  api->SetVariable("language_model_penalty_non_dict_word", "0");

  // const char* utf_string;
  // jboolean isCopy;
  // jstring fuck = "PATH";
  // utf_string = env->GetStringUTFChars( fuck, &isCopy);
  // cout << utf_string << endl;

  return;

  api->SetPageSegMode(tesseract::PSM_AUTO_OSD);
  // api->SetPageSegMode(tesseract::PSM_SPARSE_TEXT_OSD);
  api->SetImage(image);
  api->Recognize(0);

  tesseract::PageIterator* it =  api->AnalyseLayout();
  it->Orientation(&orientation, &direction, &order, &deskew_angle);

  printf("Orientation: %d;\nWritingDirection: %d\nTextlineOrder: %d\n" \
         "Deskew angle: %.4f\n",
         orientation, direction, order, deskew_angle);

  // if( orientation == 3 ){
  if( 1==1 ){

    image = pixRotate90( image, 1 );
    printf("ok, orientations is 3\n=======\n");

    api->SetImage(image);

    api->Recognize(0);
    it =  api->AnalyseLayout();
    it->Orientation(&orientation, &direction, &order, &deskew_angle);
 }

  api->SetPageSegMode(tesseract::PSM_AUTO);
  // api->SetPageSegMode(tesseract::PSM_SINGLE_COLUMN);


  // Check if change of init parameters was successful
  STRING var_value;
  api->GetVariableAsString("load_system_dawg", &var_value);
  printf("Variable 'load_system_dawg' is set to '%s'\n", var_value.string());

  outText = api->GetUTF8Text();
  printf("OCR output:\n%s", outText);

  // Destroy used object and release memory
  api->End();
  delete [] outText;
  pixDestroy(&image);

  // return 0;
}
