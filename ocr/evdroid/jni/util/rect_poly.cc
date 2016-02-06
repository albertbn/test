
#include <opencv2/opencv.hpp>
#include "angle.hpp"
#include "point.hpp"
#include "static_fields.hpp"

using namespace cv;
using namespace std;

double get_longest_side_poly ( std::vector<cv::Point> approx ) {

  Rect rect = boundingRect(approx);

  return sqrt ( rect.width*rect.width + rect.height*rect.height );
}

// TODO - go on from here - something stinky here? 6 Feb 2016
void get_closest_diagonal ( Rect rect,  Mat_<float> angles, std::vector<cv::Point> points, Mat &pic ) {

  cout << "\n\n=========\nget_closest_diagonal :: rect, angles, points\n" << rect << '\n' << angles << '\n' << points << endl;
  // vx,vy,x,y
  // (vx, vy, x0, y0), where (vx, vy) is a normalized vector collinear to the line and (x0, y0) is a point on the line
  Vec4f line_result;
  fitLine ( points, line_result, CV_DIST_WELSCH, 0, .01, .01 );

  float vx = line_result[0];
  float vy = line_result[1];
  float x = line_result[2];
  float y = line_result[3];

  cout << "get_closest_diagonal :: vx, vy, x, y : " << vx << ',' << vy << ',' << x << ',' << y << endl;

  float angle_avg = mean(angles)[0];

  float x0, y0, x1, y1;
  float larger = max(size_mat.width, size_mat.height);

  bool is_vert = is_vertical(angle_avg);
  // is_vertical

  if ( is_vert && angle_avg>=90 )
    x0 = x + vx*1.2*larger;
  else
    x0 = x - vx*1.2*larger;
  y0 = y - vy*1.2*larger;

  if ( is_vert && angle_avg>=90 )
    x1 = x - vx*1.2*larger;
  else
    x1 = x + vx*1.2*larger;

  y1 = y + vy*1.2*larger;

  // cv.Line(img, (x0-m*vx[0], y0-m*vy[0]), (x0+m*vx[0], y0+m*vy[0]), (0,0,0))

  cout << "get_closest_diagonal :: x0, y0, x1, y1 : " << x0 << ',' << y0 << ',' << x1 << ',' << y1  << endl;
  if ( lines4intersect_validate( is_vertical(angle_avg), Point(x,y), Vec4i(x0,y0,x1,y1) ) ) {
// #ifndef ANDROID
    // cv::line ( pic, Point(x0, y0), Point(x1, y1), cv::Scalar(0,64,255), 2, CV_AA ); /*boost performance*/
// #endif // ANDROID
  }
}
