
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

void filter_points_if_needed (   std::vector<cv::Point> &circles, std::vector<cv::Point> approx ){

  cv::RotatedRect rect_minAreaRect = minAreaRect(approx);
  double area1 = contourArea(approx), area_minAreaRect;
  area_minAreaRect = rect_minAreaRect.size.width * rect_minAreaRect.size.height;
  // std::cout << "area: " << area1 << " ,minAreaRect area:" << area_minAreaRect  << std::endl;
  // DONE - go on from here... - divide the area (width * height of the minArea) and then divide by the contourArea - and see if result is (>2,3,4) larger than say 2...
  bool is_bouble_hollow_shape = (area_minAreaRect/area1)>3.0;

  if(!is_bouble_hollow_shape) return;

  // if you got to here, it's a hollow rectangle - like ===\\ and should be ---\
  // that is, remove double close ~90 degree angles...
  std::vector<cv::Point2f> circle_points; /*need to convert to Point2f for kmeans */
  for ( int i=0; i<(int)circles.size(); ++i ) {
    circle_points.push_back(Point2f(circles[i].x, circles[i].y));
  }

  int clusterCount = ceil((float)circles.size()/2.0);
  int attempts = 1;
  Mat llabels, centers;
  std::vector<int> labels_dummy;
  kmeans(circle_points, clusterCount, llabels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 100, 0.0001), attempts, KMEANS_PP_CENTERS, centers );
  // std::cout << "\n\n ~~~~ filter points ~~~~ \n\n labels: " << llabels << "centers" << centers << "circle_points" << circle_points << std::endl;

  std::vector<cv::Point> circles_filtered;
  Mat_<int> labels = llabels;
  for ( int j=0; j<llabels.rows; ++j ) {

    if ( std::find(labels_dummy.begin(), labels_dummy.end(), labels(j,0)) != labels_dummy.end() )
      continue;

    labels_dummy.push_back(labels(j,0));
    circles_filtered.push_back(circles[j]);
  }

  circles = circles_filtered;
}
