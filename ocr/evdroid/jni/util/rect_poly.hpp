
/*! \file rect_poly.hpp */

#ifndef RECT_POLY_HPP
#define RECT_POLY_HPP

#ifdef __cplusplus

#ifndef SKIP_INCLUDES
// includes here
#include <opencv2/core/core.hpp>
#endif // SKIP_INCLUDES

using namespace cv;
using namespace std;

double get_longest_side_poly ( std::vector<cv::Point> approx );

void get_closest_diagonal ( Mat_<float> angles, vector< std::vector<cv::Point> > contours, Mat &pic, vector<double> len_contours );

#endif // __cplusplus
#endif /*RECT_POLY_HPP*/

