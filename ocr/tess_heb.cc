// compile
// g++ -o tess_heb tess_heb.cc -llept -ltesseract && ./tess_heb pics/heb.jpg

// Fred Weinhaus image ready
// http://www.fmwconcepts.com/imagemagick/textcleaner/index.php
// ./textcleaner -g -e none -f 100 -o 3 pics/heb.jpg ./img_pre/heb.jpg

#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>
#include "tesseract/strngs.h"
#include "tesseract/genericvector.h"

int main ( int argc, char** argv )
{
  argc = 0;

  PIX *image = pixRead(argv[1]); /*send picture as first argument*/

  tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
  printf("Using tesseract c++ API: %s\n", api->Version());

  // turn of dictionaries -> only possible during init
  GenericVector<STRING> vars_vec;
  vars_vec.push_back("load_system_dawg");
  vars_vec.push_back("load_freq_dawg");
  vars_vec.push_back("load_punc_dawg");
  vars_vec.push_back("load_number_dawg");
  vars_vec.push_back("load_unambig_dawg");
  vars_vec.push_back("load_bigram_dawg");
  vars_vec.push_back("load_fixed_length_dawgs");
  vars_vec.push_back("user_patterns_suffix");

  GenericVector<STRING> vars_values;
  vars_values.push_back("F");
  vars_values.push_back("F");
  vars_values.push_back("F");
  vars_values.push_back("F");
  vars_values.push_back("F");
  vars_values.push_back("F");
  vars_values.push_back("F");
  vars_values.push_back("pharma-words");

  // api->Init( NULL, "heb" );
  // credits: zdentop, thanks? http://pastebin.com/qxUPEQZm
  // for documentation see: http://tesseract-ocr.github.io/a01278.html#a04550a0ed1279562027bf2fc92c421ae

  api->Init(NULL, "heb", tesseract::OEM_DEFAULT ,
            NULL, 0, &vars_vec, &vars_values, false);
  api->SetVariable("language_model_penalty_non_dict_word", "0");

  // api->SetPageSegMode(tesseract::PSM_AUTO);
  api->SetPageSegMode(tesseract::PSM_SINGLE_COLUMN);
  api->SetImage(image);
  api->SetVariable("save_blob_choices", "T");
  api->Recognize(NULL);

  tesseract::ResultIterator* ri = api->GetIterator();
  tesseract::PageIteratorLevel level = tesseract::RIL_SYMBOL;
  if(ri != 0) {
    do {
      const char* symbol = ri->GetUTF8Text(level);
      float conf = ri->Confidence(level);
      if(symbol != 0) {
        printf("symbol %s, conf: %f", symbol, conf);
        bool indent = false;
        tesseract::ChoiceIterator ci(*ri);
        do {
          if (indent) printf("\t\t ");
          printf("\t- ");
          const char* choice = ci.GetUTF8Text();
          printf("%s conf: %f\n", choice, ci.Confidence());
          indent = true;
        } while(ci.Next());
      }
      printf("---------------------------------------------\n");
      delete[] symbol;
    } while((ri->Next(level)));
  }

  pixDestroy(&image);

  return 0;
}
