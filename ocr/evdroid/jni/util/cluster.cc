
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include "angle.hpp"
#include "cluster.hpp"
#include "common.hpp"
#include "rect_poly.hpp"
#include "static_fields.hpp"

using namespace cv;
using namespace std;

Mat coord_clusters ( Size size, std::vector < std::vector<cv::Point> > contours, Mat_<float> angles, double angle_center, std::vector<double> len_contours ){

  bool is_vert = is_vertical(angle_center);
  std::vector<cv::Point2f> points;
  for ( int i=0; i<(int)contours.size(); ++i ) {

    if ( is_vert ) {
      points.push_back( Point2f(contours[i][0].x, 0) );
    }
    else {
      points.push_back( Point2f(0, contours[i][0].y) );
    }
  }

  int clusterCount =  2;
  int attempts = 1;
  Mat llabels, centers;
  kmeans ( points, clusterCount, llabels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 100, 0.0001), attempts, KMEANS_PP_CENTERS, centers );

  std::vector < std::vector<cv::Point> > contours_l0, contours_l1;

  Mat_<double> centersd = centers;
  Mat_<int> labels = llabels;  Mat_<float> angles0, angles1;
  double xy_centre_diff = is_vert ? abs(centersd(0,0)-centersd(1,0)) : abs(centersd(0,1)-centersd(1,1));

  // this here means there is just 1 group of hor or vert lines
  if( get_max_deviation(size, angle_center, is_vert) > xy_centre_diff ){
    for ( int j=0; j<labels.rows; ++ j ) {
      labels(j,0) = 0;
    }
  }

  // std::cout << "\n\n ~~~~ coord clusters ~~~~ \n\n labels: is vert: "<< is_vert << '\n' << llabels << "centers" << centers << "points" << points << '\n' << "xy_centre_diff: " << xy_centre_diff << std::endl;

  std::vector<double> len_contours0, len_contours1;

  for ( int j=0; j<labels.rows; ++j ) {
    if(labels(j,0)==0){
      contours_l0.push_back(contours[j]); angles0.push_back(angles(j,0));
      len_contours0.push_back(len_contours[j]);
      // len_contours0.push_back( arcLength(contours[j],true));
    } else if(labels(j,0)==1){
      contours_l1.push_back(contours[j]); angles1.push_back(angles(j,0));
      len_contours1.push_back(len_contours[j]);
      // len_contours1.push_back(arcLength(contours[j],true));
    }
  }

  // go on from here - check for size > 1 maybe..., declare fn below...
  if( (int)contours_l0.size()>0) reduce_noise_short_lines( contours_l0, angles0, len_contours0 );
  if( (int)contours_l1.size()>0) reduce_noise_short_lines( contours_l1, angles1, len_contours1 );

  return coord_clusters_munge( size, contours_l0, contours_l1, angles0, angles1, len_contours0, len_contours1 );
}

// probably the closest diagonal - fit line
Mat coord_clusters_munge ( Size size,
                           std::vector < std::vector<cv::Point> > contours_l0,
                           std::vector < std::vector<cv::Point> > contours_l1,
                           Mat_<float> angles0,
                           Mat_<float> angles1,
                           vector<double> len_contours0,
                           vector<double> len_contours1
                           ) {
  Mat l0, l1;

// #ifndef ANDROID
  // if file exists - load from it - else create from zeros existing one...
  if(contours_l0.size()>0){ /*boost performance*/
    if(file_exists(path_img + "/long5.jpg"))
      l0 = imread(path_img + "/long5.jpg");
    else
      l0 = Mat::zeros( size, CV_8UC3 );
  }
  // the second one is optional
  if(contours_l1.size()>0){

    if(file_exists(path_img + "/long6.jpg"))
      l1 = imread(path_img + "/long6.jpg");
    else
      l1 = Mat::zeros( size, CV_8UC3 );
  }

  if(contours_l0.size()>0) /*boost performance*/
    cv::drawContours(l0, contours_l0, -1, cv::Scalar(0,255,0),1);
  // the second one is optional
  if(contours_l1.size()>0)
    cv::drawContours(l1, contours_l1, -1, cv::Scalar(255,255,0),1);
// #endif // ANDROID

  if ( contours_l0.size()>0 ) {
    get_closest_diagonal ( angles0, contours_l0, l0, len_contours0); /*closest diagonal*/
// #ifndef ANDROID
    cv::imwrite( path_img + "/long5.jpg", l0);
// #endif //ANDROID
  }

  // the second one is optional
  if ( contours_l1.size()>0 ) {
    get_closest_diagonal( angles1, contours_l1, l1, len_contours1); /*closest diagonal*/
// #ifndef ANDROID
    cv::imwrite( path_img + "/long6.jpg", l1);
// #endif //ANDROID
  }

  return l0; /*dummy*/
}
