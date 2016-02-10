
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

bool is_vertical ( float angle );

float angle_2points ( cv::Point p1, cv::Point p2, float & );

int get_angle_approx90_count ( std::vector<cv::Point> approx, Mat drawing, std::vector<cv::Point>& points4 );

Mat angle_clusters( std::vector < std::vector<cv::Point> > contours, Mat_<float> &angles, Mat_<double> &centers );


#endif // __cplusplus
#endif /*ANGLE_HPP*/

