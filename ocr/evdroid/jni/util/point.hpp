
/*! \file point.hpp */

#ifndef POINT_HPP
#define POINT_HPP

#ifdef __cplusplus

#ifndef SKIP_INCLUDES
// includes here
#include <opencv2/core/core.hpp>
#endif // SKIP_INCLUDES

using namespace std;

void filter_points_if_needed (   std::vector<cv::Point> &circles, std::vector<cv::Point> approx );

#endif // __cplusplus
#endif /*POINT_HPP*/

