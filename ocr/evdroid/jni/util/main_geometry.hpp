
/*! \file main_geometry.hpp */

#ifndef MAIN_GEOMETRY_HPP
#define MAIN_GEOMETRY_HPP

#ifdef __cplusplus

#ifndef SKIP_INCLUDES
// includes here
#include <opencv2/core/core.hpp>
#endif // SKIP_INCLUDES

using namespace cv;
using namespace std;

void longest_closed();

void deal_with_geometry_when_not_enough_90d_angles (
                                                          Size mat_size,
                                                          std::vector<std::vector<cv::Point> > contoursDraw2,
                                                          std::vector<double> len_contours_contoursDraw2,
                                                          double min_line_length
                                                           );
void split_contours_2_dotted_lines( std::vector<std::vector<cv::Point> > &contoursDraw2, std::vector<double> &len_contours_contoursDraw2, double min_line_length );

void final_magic_crop_rotate ( Mat mat,  std::vector<cv::Point>& points4 );

#endif // __cplusplus
#endif /*MAIN_GEOMETRY_HPP*/

