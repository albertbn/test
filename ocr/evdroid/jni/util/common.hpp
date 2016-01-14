
/*! \file common.hpp */

#ifndef COMMON_HPP
#define COMMON_HPP

#ifdef __cplusplus

#ifndef SKIP_INCLUDES
#include <string>
#include <stdbool.h>
#include <opencv2/core/core.hpp>

#endif // SKIP_INCLUDES

using namespace cv;
using namespace std;

bool file_exists ( const string& name );

double get_max_deviation(Size size, double angle_center, bool is_vert);

void reduce_noise_short_lines ( std::vector < std::vector<cv::Point> > &contours, Mat_<float> &angles, std::vector<double> len_contours );

#endif // __cplusplus
#endif /*COMMON_HPP*/

