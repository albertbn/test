
/*! \file static_fields.hpp */

#ifndef STATIC_FIELDS
#define STATIC_FIELDS

#ifdef __cplusplus

#ifndef SKIP_INCLUDES
// includes here
#include <opencv2/core/core.hpp>
#endif // SKIP_INCLUDES

using namespace cv;
using namespace std;

static std::vector<cv::Vec4i> lines4intersect;
static std::vector<bool> lines4intersect_is_vert;
static std::vector<Point> p_from_line_vector;
static cv::Point center(0,0);
static Size size_mat;
static double MIN_LINE_LENGTH_CONSIDERED_SIDE;
static const char* path_sd_card;

#endif // __cplusplus
#endif /*STATIC_FIELDS*/

