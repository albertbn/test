
/*! \file angle.hpp */

#ifndef ANGLE_HPP
#define ANGLE_HPP

#ifdef __cplusplus

#ifndef SKIP_INCLUDES
#include <opencv2/core/core.hpp>
#endif // SKIP_INCLUDES

using namespace cv;
using namespace std;

double angle ( cv::Point pt1, cv::Point pt2, cv::Point pt0 );
int get_angle_approx90_count ( std::vector<cv::Point> approx, Mat drawing, std::vector<cv::Point>& points4 );

#endif // __cplusplus
#endif /*ANGLE_HPP*/

