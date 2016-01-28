
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include "rect_poly.hpp"
#include "point.hpp"

// hi from new frame :) the force

using namespace cv;
using namespace std;

double angle ( cv::Point pt1, cv::Point pt2, cv::Point pt0 ) {
        double dx1 = pt1.x - pt0.x;
        double dy1 = pt1.y - pt0.y;
        double dx2 = pt2.x - pt0.x;
        double dy2 = pt2.y - pt0.y;
        double cos_value = (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);

        return acos(cos_value) * 180.0 / CV_PI;
}

// is angle considered vertical
bool is_vertical ( float angle ) {
  /*between 45 degrees trough 90 degrees to 135 degrees is considered vertical*/
  return angle > 45.0 && angle < 135.0;
}

float angle_2points ( cv::Point p1, cv::Point p2 ) {

  float ang = atan2(p1.y - p2.y, p1.x - p2.x);
  ang = abs(ang * 180 / CV_PI);

  /*put all folks in the top left quadrant of the 360 circle - used only for kmeans clustering*/
  ang<90.0 && (ang=180.0-ang);
  return ang;
}

int get_angle_approx90_count ( std::vector<cv::Point> approx, Mat drawing, std::vector<cv::Point>& circles /*points4*/  ) {

  int vtc = approx.size();

  double ang, ang_deg;
  int angle90_count = 0;

  int j_mid;
  for ( int j = 1; j < vtc+1; j++ ) {

    (j==1 && (j_mid = vtc-1)) || (j_mid=j-2);
    ang = ang_deg = angle(approx[j%vtc], approx[j_mid], approx[j-1]);

    if ( ang_deg >60.0 && ang_deg<120.0 ) {
      ++angle90_count;
      circles.push_back(approx[j-1]);
    }
  }

  double diag = 0;
  angle90_count && (diag = get_longest_side_poly ( circles ));

  if ( angle90_count ) {

    if ( diag>100 ) {
      filter_points_if_needed(circles, approx);
      angle90_count = circles.size();

#ifndef ANDROID
      // std::cout << "OK, drawing circles... " << clen << std::endl;
      for ( int j=0; j<angle90_count; ++j ) { /*boost performance*/
        cv::circle ( drawing, circles[j], 50,  cv::Scalar(50,0,255) );
      }
#endif // ANDROID

    }
  }

  return angle90_count;
}

Mat angle_clusters( std::vector < std::vector<cv::Point> > contours, Mat_<float> &angles, Mat_<double> &centers ){

  int clusterCount = 2;
  Mat labels;
  Mat_<int> labels2;

  // Mat angles;
  for ( int i=0; i<(int)contours.size(); ++i ) {
    angles.push_back ( angle_2points(contours[i][0], contours[i][1]) );
  }

  // check if there was just one line - then return here
  if ( (int)contours.size() < clusterCount ) {
    labels2.push_back(0);
    centers.push_back(angles.at<float>(0,0));
    return labels2;
  }

  int attempts = 5;
  kmeans(angles, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers );

  labels2 = labels;
  double angle_centre_diff = abs(centers(0,0)-centers(1,0));

  if ( angle_centre_diff > 170.0 || angle_centre_diff<10.0 ){
    for ( int j=0; j<labels.rows; ++ j ) {
      labels2(j,0) = 0;
    }
  }

  return labels2;
}
