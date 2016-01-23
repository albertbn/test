
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

extern unsigned int clock_start;
extern ofstream outfile;
extern std::vector<cv::Vec4i> lines4intersect;
extern std::vector<bool> lines4intersect_is_vert;
extern std::vector<Point> p_from_line_vector;
extern cv::Point center;
extern Size size_mat;
extern double MIN_LINE_LENGTH_CONSIDERED_SIDE;
extern string path_sd_card;
extern string path_img;

#endif // __cplusplus
#endif /*STATIC_FIELDS*/

