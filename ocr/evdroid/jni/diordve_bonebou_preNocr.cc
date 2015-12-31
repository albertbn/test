
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

#include <iostream>
#include <stdlib.h>
#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>
#include "tesseract/strngs.h"
#include "tesseract/genericvector.h"

using namespace std;

JNIEXPORT void JNICALL Java_diordve_bonebou_preNocr_doit (
     JNIEnv *env, jobject thisObj, jstring jtessdata_path_pref ) {
// int main ( ) {
  char tessdata_path_pref_str[128];
  const char* tessdata_path_pref = (*env).GetStringUTFChars(jtessdata_path_pref, 0);
  strcpy(tessdata_path_pref_str, tessdata_path_pref);
  strcat(tessdata_path_pref_str,"tessdata/long8.jpg");
  // const char* inputfile = tessdata_path_pref + "long8.jpg";
  const char* inputfile = tessdata_path_pref_str;

  char *outText = NULL;
  tesseract::Orientation orientation;
  tesseract::WritingDirection direction;
  tesseract::TextlineOrder order;
  float deskew_angle;

  tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
  printf("Using tesseract c++ API: %s\n", api->Version());

  return;
  
  PIX *image = pixRead(inputfile);

  GenericVector<STRING> vars_vec;
  vars_vec.push_back("load_system_dawg");

  GenericVector<STRING> vars_values;
  vars_values.push_back("F");

  api->Init(tessdata_path_pref, "heb", tesseract::OEM_DEFAULT , NULL, 0, &vars_vec, &vars_values, false);
  // cout << api->GetDatapath() << endl;

  api->SetVariable("language_model_penalty_non_dict_word", "0");

  api->SetPageSegMode(tesseract::PSM_AUTO_OSD);
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

  (*env).ReleaseStringUTFChars(jtessdata_path_pref, tessdata_path_pref);
  // return 0;
}
