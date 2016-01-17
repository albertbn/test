
/*! \file tess.hpp */

#ifndef TESS_HPP
#define TESS_HPP

#ifdef __cplusplus

#ifndef SKIP_INCLUDES
// includes here
#include <opencv2/opencv.hpp>
// #include <tesseract/baseapi.h>
#include <baseapi.h>

#endif // SKIP_INCLUDES

using namespace cv;
// using namespace std;

extern tesseract::TessBaseAPI tess;
void init_ocr();
void crop_b_tess ( Mat mat/*orig*/, Rect rect );
void rot90 ( cv::Mat &matImage, int rotflag );
void orientation_check ( Mat& mat );

#endif // __cplusplus
#endif /*TESS_HPP*/

