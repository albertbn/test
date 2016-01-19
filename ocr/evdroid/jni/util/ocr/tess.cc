
#ifdef ANDROID
#include <baseapi.h>
#include <genericvector.h>
#else
#include <tesseract/baseapi.h>
#include <tesseract/genericvector.h>
#endif // ANDROID

#include "tess.hpp"
#include "../static_fields.hpp"

tesseract::TessBaseAPI tess;
tesseract::Orientation orientation;
tesseract::WritingDirection direction;
tesseract::TextlineOrder order;
float deskew_angle;

// using namespace cv;

void init_ocr ( ) {

  GenericVector<STRING> vars_vec;
  vars_vec.push_back ( "load_system_dawg" );

  GenericVector<STRING> vars_values;
  vars_values.push_back ( "F" );

  tess.Init ( path_sd_card.c_str(), "heb", tesseract::OEM_DEFAULT );
  // tess.Init( path_sd_card.c_str(), "heb", tesseract::OEM_DEFAULT , NULL, 0, &vars_vec, &vars_values, false);

  tess.SetPageSegMode ( tesseract::PSM_AUTO_OSD ); /*further down change back the page mode to text detection*/
}

void crop_b_tess ( Mat mat/*orig*/, Rect rect ) {

  Mat cropped = mat(rect).clone(); /*!clone, clone clone*/

  // Pass it to Tesseract API
  tess.SetImage ( (uchar*)cropped.data, cropped.cols, cropped.rows, 1, cropped.cols );

  // Get the text
  char* out = tess.GetUTF8Text();
  outfile << out;
  // std::cout << out;
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
