
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

  cout << "\n\n=========\nget_closest_diagonal :: rect, angles, points " << rect << ',' << angles << ',' << points << endl;
  // vx,vy,x,y
  // (vx, vy, x0, y0), where (vx, vy) is a normalized vector collinear to the line and (x0, y0) is a point on the line
  Vec4f line_result;
  fitLine ( points, line_result, CV_DIST_L2, 0, .01, .01 );

  float vx = line_result[0];
  float vy = line_result[1];
  float x = line_result[2];
  float y = line_result[3];

  float dd[]= {vx,vy,x,y};
  cout << "get_closest_diagonal :: vx, vy, x, y : " << dd << endl;

  float angle_avg = mean(angles)[0];

  float x0, y0, x1, y1;

  x0 = x - vx*(2*size_mat.width);
  y0 = y - vy*(2*size_mat.height);

  x1 = x + vx*(2*size_mat.width);
  y1 = y + vy*(2*size_mat.height);

  float dd2[] = {x0,y0,x1,y1};
  cout << "get_closest_diagonal :: x0, y0, x1, y1 : " << dd2 << endl;
  if ( lines4intersect_validate( is_vertical(angle_avg), Point(x,y), Vec4i(x0,y0,x1,y1) ) ) {
// #ifndef ANDROID
    cv::line ( pic, Point(x0, y0), Point(x1, y1), cv::Scalar(0,64,255), 2, CV_AA ); /*boost performance*/
// #endif // ANDROID
  }
}
