
/*! \file close_center.hpp */

#ifndef CLOSE_CENTER_HPP
#define CLOSE_CENTER_HPP

#ifdef __cplusplus

#ifndef SKIP_INCLUDES
#include <opencv2/core/core.hpp>
#endif // SKIP_INCLUDES

using namespace cv;
using namespace std;

struct less_custom_sort_points;

void sort_points_closest_2center8 ( std::vector<cv::Point>& points4x );

void sort_points_closest_2center (  std::vector<cv::Point>& points4 );

void additional_logic_closest2center ( std::vector<cv::Point>& points4, std::vector<cv::Point>& points40, std::vector<cv::Point>& points41, int tl_br_index );

#endif // __cplusplus
#endif /*CLOSE_CENTER_HPP*/

