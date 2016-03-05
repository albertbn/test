
/*! \file multipleObjectTracking.hpp */

#ifndef MULTIPLEOBJECTTRACKING_HPP
#define MULTIPLEOBJECTTRACKING_HPP

#ifdef __cplusplus

#ifndef SKIP_INCLUDES
#include <opencv2/opencv.hpp>
// #include <opencv2/core/core.hpp>
#endif // SKIP_INCLUDES

void save_middle_class ( cv::Mat picture );
void do_frame( cv::Mat cameraFeed );

int maina();

#endif // __cplusplus
#endif /*MULTIPLEOBJECTTRACKING_HPP*/
