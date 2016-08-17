#ifdef ANDROID
#include <baseapi.h>
#include <genericvector.h>
#include <ocrclass.h> // ETEXT_DESC
#else
#include <tesseract/baseapi.h>
#include <tesseract/genericvector.h>
#include <tesseract/ocrclass.h>
#endif // ANDROID

#include <unistd.h>
#include "tess.hpp"
#include "../static_fields.hpp"
// #include "../common.hpp"

tesseract::TessBaseAPI tess;
tesseract::Orientation orientation;
tesseract::WritingDirection direction;
tesseract::TextlineOrder order;
float deskew_angle;

// using namespace cv;

template <class T>
inline string to_string ( const T& t ) {

  std::stringstream ss;
  ss << t;
  return ss.str();
}

void init_ocr ( ) {

  GenericVector<STRING> vars_vec;
  ////vars_vec.push_back("load_system_dawg");
  // vars_vec.push_back("load_freq_dawg");
  ////vars_vec.push_back("load_punc_dawg");
  ////vars_vec.push_back("load_number_dawg");
  // vars_vec.push_back("load_unambig_dawg");
  // vars_vec.push_back("load_bigram_dawg");
  // vars_vec.push_back("load_fixed_length_dawgs");
  // vars_vec.push_back("user_patterns_suffix");

  GenericVector<STRING> vars_values;
  ////vars_values.push_back("F");
  // vars_values.push_back("F");
  //// vars_values.push_back("F");
  //// vars_values.push_back("F");
  // vars_values.push_back("F");
  // vars_values.push_back("F");
  // vars_values.push_back("F");
  // vars_values.push_back("pharma-words");

#ifdef ANDROID
  // tess.Init ( path_sd_card.c_str(), "heb", tesseract::OEM_DEFAULT );
  tess.Init ( path_sd_card.c_str(), "heb+eng", tesseract::OEM_DEFAULT , NULL, 0, &vars_vec, &vars_values, false );
#else
  // tess.Init ( NULL, "heb", tesseract::OEM_DEFAULT ); /*regular exe computer*/
  tess.Init ( NULL, "heb+eng", tesseract::OEM_DEFAULT , NULL, 0, &vars_vec, &vars_values, false );
#endif // ANDROID

  outfile << "tess version: " << tess.Version() << '\n';

  // tess.SetPageSegMode ( tesseract::PSM_AUTO_OSD ); /*further down change back the page mode to text detection*/
}

void crop_b_tess ( Mat& mat/*orig*/, Rect& rect, int icount, int fd[] ) {


  // init_ocr();
  // unsigned int _clock_start = clock();

  // Mat cropped = mat(rect).clone(); /*!clone, clone clone*/
  Mat cropped ( mat(rect) ); /*!clone, clone clone*/

  // Pass it to Tesseract API
  tess.SetImage ( (uchar*)cropped.data, cropped.cols, cropped.rows, 1, cropped.cols );

  // Get the text
  char* out = tess.GetUTF8Text();

  // if ( icount<1 ) outfile << "CCLLEEAARR";
  // outfile_ocr << out;

  if ( icount<1 ){

    //   write ( fd[1], "CCLLEEAARR", sizeof("CCLLEEAARR") );
  }

  write ( fd[1], out, strlen(out)+1 );

  delete[] out;

  cropped.release();

  // tess.Clear();
  // tess.End();
}

void rot90 ( cv::Mat &matImage, int rotflag ) {

  //1=CW, 2=CCW, 3=180
  if (rotflag == 1){
    transpose(matImage, matImage);
    flip(matImage, matImage,1); //transpose+flip(1)=CW
  } else if (rotflag == 2) {
    transpose(matImage, matImage);
    flip(matImage, matImage,0); //transpose+flip(0)=CCW
  } else if (rotflag ==3){
    flip(matImage, matImage,-1);    //flip(-1)=180
  } else if (rotflag != 0){ //if not 0,1,2,3:
    std::cout  << "Unknown rotation flag(" << rotflag << ")" << std::endl;
  }
}

void orientation_check ( Mat& mat ) {

  tesseract::PageIterator* it =  tess.AnalyseLayout();
  it->Orientation(&orientation, &direction, &order, &deskew_angle);
  printf("Orientation: %d;\nWritingDirection: %d\nTextlineOrder: %d\n" \
         "Deskew angle: %.4f\n",
         orientation, direction, order, deskew_angle);

  if( orientation==3 ){
    rot90(mat, 1);
  } else if(orientation==2){
    rot90(mat,3);
  } else if(orientation==1){
    rot90(mat,2);
  }
}
