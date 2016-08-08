
/*! \file cluster.hpp */

#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#ifdef __cplusplus

#ifndef SKIP_INCLUDES
#include <opencv2/core/core.hpp>
#endif // SKIP_INCLUDES

using namespace cv;
using namespace std;

Mat coord_clusters( Size size, std::vector < std::vector<cv::Point> > contours, Mat_<float> angles, double angle_center, std::vector<double> len_contours );

Mat coord_clusters_munge ( Size size,
                           std::vector < std::vector<cv::Point> > contours_l0,
                           std::vector < std::vector<cv::Point> > contours_l1,
                           Mat_<float> angles0, Mat_<float> angles1,
                           vector<double> len_contours0, vector<double> len_contours1
                           );

#endif // __cplusplus
#endif /*CLUSTER_HPP*/

