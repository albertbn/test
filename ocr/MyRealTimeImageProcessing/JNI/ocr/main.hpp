
/*! \file main.hpp */

#ifndef MAIN_HPP
#define MAIN_HPP

#ifdef __cplusplus

#ifndef SKIP_INCLUDES
// includes here
#include <fstream>
#include <opencv2/opencv.hpp>

#endif // SKIP_INCLUDES

using namespace cv;
// using namespace std;

void ocr_doit ( Mat& im_orig, std::ofstream outfile_ocr );
std::vector<cv::Rect> detectLetters ( cv::Mat img );

#endif // __cplusplus
#endif /*MAIN_HPP*/

