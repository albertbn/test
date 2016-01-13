
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include "rect_poly.hpp"
#include "point.hpp"

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
      // std::cout << "OK, drawing circles... " << clen << std::endl;
      for ( int j=0; j<angle90_count; ++j ) {
        cv::circle ( drawing, circles[j], 50,  cv::Scalar(50,0,255) );
      }
    }
  }

  return angle90_count;
}
