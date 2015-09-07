
// this folk is in c++, yep!

#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>
#include "tesseract/strngs.h"
#include "tesseract/genericvector.h"
#include "tesseract/genericvector.h"

int main()
{
  const char* inputfile = "./pics/tj.jpg";
  char *outText = NULL;
  // const char* inputfile = "./pics/tj2.jpg";
  tesseract::Orientation orientation;
  tesseract::WritingDirection direction;
  tesseract::TextlineOrder order;
  float deskew_angle;

  PIX *image = pixRead(inputfile);
  tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
  printf("Using tesseract c++ API: %s\n", api->Version());

  // turn of dictionaries -> only possible during init
  GenericVector<STRING> vars_vec;
  vars_vec.push_back("load_system_dawg");
  // vars_vec.push_back("load_freq_dawg");
  // vars_vec.push_back("load_punc_dawg");
  // vars_vec.push_back("load_number_dawg");
  // vars_vec.push_back("load_unambig_dawg");
  // vars_vec.push_back("load_bigram_dawg");
  // vars_vec.push_back("load_fixed_length_dawgs");
  //vars_vec.push_back("user_patterns_suffix");

  GenericVector<STRING> vars_values;
  vars_values.push_back("F");
  // vars_values.push_back("F");
  // vars_values.push_back("F");
  // vars_values.push_back("F");
  // vars_values.push_back("F");
  // vars_values.push_back("F");
  // vars_values.push_back("F");
  //vars_values.push_back("pharma-words");

  // api->Init( NULL, "eng" );
  // credits: zdentop, thanks? http://pastebin.com/qxUPEQZm
  // for documentation see: http://tesseract-ocr.github.io/a01278.html#a04550a0ed1279562027bf2fc92c421ae
  api->Init(NULL, "eng", tesseract::OEM_DEFAULT ,
            NULL, 0, &vars_vec, &vars_values, false);
  // api->SetVariable("tessedit_char_whitelist",
  //                  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
  //                  "0123456789,®");
  // api->SetVariable("language_model_penalty_non_dict_word", "0");

  api->SetPageSegMode(tesseract::PSM_AUTO_OSD);
  api->SetImage(image);
  api->Recognize(0);

  tesseract::PageIterator* it =  api->AnalyseLayout();
  it->Orientation(&orientation, &direction, &order, &deskew_angle);

  printf("Orientation: %d;\nWritingDirection: %d\nTextlineOrder: %d\n" \
         "Deskew angle: %.4f\n",
         orientation, direction, order, deskew_angle);

  if( orientation == 3 ){

    image = pixRotate90( image, 1 );
    printf("ok, orientations is 3\n=======\n");

    api->SetImage(image);
  }

  if( deskew_angle !=0  ){
    int redsearch = 2;
    printf( "making pixDeskew... %d\n", redsearch );
    image = pixDeskew(image,2);
    api->SetImage(image);
  }

  api->SetPageSegMode(tesseract::PSM_AUTO);


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

  return 0;
}


// int main()
// {
//     char *outText;

//     tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
//     // Initialize tesseract-ocr with English, without specifying tessdata path
//     if ( api->Init(NULL, "eng") )  {
//         fprintf(stderr, "Could not initialize tesseract.\n");
//         exit(1);
//     }

//     // Open input image with leptonica library
//     // Pix *image = pixRead("./pics/tj2.jpg");
//     Pix *image = pixRead("./pics/tj.jpg");
//     api->SetImage(image);
//     // Get OCR result
//     outText = api->GetUTF8Text();
//     printf("OCR output:\n%s", outText);

//     // Destroy used object and release memory
//     api->End();
//     delete [] outText;
//     pixDestroy(&image);

//     return 0;
// }
