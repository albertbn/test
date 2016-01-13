
/*! \file point.hpp */

#ifndef POINT_HPP
#define POINT_HPP

#ifdef __cplusplus

#ifndef SKIP_INCLUDES
// includes here
#include <opencv2/core/core.hpp>
#endif // SKIP_INCLUDES

using namespace cv;
using namespace std;

void filter_points_if_needed (   std::vector<cv::Point> &circles, std::vector<cv::Point> approx );

bool lines4intersect_validate(bool is_vert, Point p_from_line, Vec4i vec4i);

bool intersection(Point2f o1, Point2f p1, Point2f o2, Point2f p2, Point2f &r);

cv::Point computeIntersect ( cv::Vec4i a, cv::Vec4i b );

void intersect_n_get_points ( std::vector<cv::Point>& points4  );

bool sortCorners ( std::vector<cv::Point>& corners, cv::Point center );

bool corners_magick_do ( Size mat_size, std::vector<cv::Point>& corners /*points4*/ );

#endif // __cplusplus
#endif /*POINT_HPP*/

