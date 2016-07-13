
/*! \file common.hpp */

#ifndef COMMON_HPP
#define COMMON_HPP

#ifdef __cplusplus

#ifndef SKIP_INCLUDES
#include <string>
#include <stdbool.h>
#include <opencv2/core/core.hpp>

// REMARK for mac debug
#include <android/log.h>
#define LOG_TAG "evdroid"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))

#endif // SKIP_INCLUDES

using namespace cv;
using namespace std;

float clock_ticks_to_ms ( unsigned int ticks );

template <class T>
std::string to_string ( const T& t );

bool file_exists ( const string& name );

bool directory_exists ( string path );

double get_max_deviation(Size size, double angle_center, bool is_vert);

void reduce_noise_short_lines (
                               std::vector < std::vector<cv::Point> > &contours,
                               Mat_<float> &angles,
                               std::vector<double> len_contours
                               );

void split_lines_analyze_n_reduce_background_noise (
                                              std::vector<cv::Point> line_poly,
                                              std::vector<std::vector<cv::Point> > &contoursDraw3,
                                              std::vector<double> &len_contours_contoursDraw
                                              );

void mat_downscale_check ( Mat &mat );

#endif // __cplusplus
#endif /*COMMON_HPP*/

