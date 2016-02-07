
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

vector<cv::Point> get_points_from_contours ( bool is_vert, vector< std::vector<cv::Point> > contours ) {

  //TODO - filter out lines by vert/hor out of standard dev
  vector<cv::Point> points;

  for ( int i=0; i<(int)contours.size(); ++i ) {
    if ( contours[i][0].x==0 || contours[i][1].x==0  ) continue;
    points.push_back(contours[i][0]);
    points.push_back(contours[i][1]);
  }

  return points;
}

float get_angle_avg_by_lengths ( Mat_<float> angles,  vector<double> len_contours ) {

  float angles_sum_by_len = 0.0;
  double divide_by=0;
  for ( int i=0; i<angles.rows; ++i ) {
    angles_sum_by_len+=angles[i][0]*len_contours[i];
    divide_by+=len_contours[i];
  }

  return angles_sum_by_len/divide_by;
}

// TODO - go on from here - something stinky here? 6 Feb 2016
void get_closest_diagonal ( Mat_<float> angles, vector< std::vector<cv::Point> > contours, Mat &pic, vector<double> len_contours ) {

  float angle_avg = mean(angles)[0]; /*calculate, to know if horizontal or vertical*/
  bool is_vert = is_vertical(angle_avg);

  vector<cv::Point> points = get_points_from_contours(is_vert, contours);

  cout << "\n\n=========\nget_closest_diagonal :: angles, points\n" << '\n' << angles << '\n' << points << endl;
  // vx,vy,x,y
  // (vx, vy, x0, y0), where (vx, vy) is a normalized vector collinear to the line and (x0, y0) is a point on the line
  Vec4f line_result;
  fitLine ( points, line_result, CV_DIST_WELSCH, 0, .01, .01 );

  float vx = line_result[0];
  float vy = line_result[1];
  float x = line_result[2];
  float y = line_result[3];

  float angle__avg_by_len =  get_angle_avg_by_lengths ( angles, len_contours );
  //TODO - go on from x = 1291 * atan2(180-angle... vert hor rad - see get_max_deviation in common)
  cout << "get_closest_diagonal :: angle__avg_by_len : " << angle__avg_by_len << endl;
  cout << "get_closest_diagonal :: vx, vy, x, y : " << vx << ',' << vy << ',' << x << ',' << y << endl;

  float x0, y0, x1, y1;
  float larger = max(size_mat.width, size_mat.height);

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

  cv::circle ( pic, Point(x,y), 50, cv::Scalar(255,255,255) );

  cout << "get_closest_diagonal :: x0, y0, x1, y1 : " << x0 << ',' << y0 << ',' << x1 << ',' << y1  << endl;
  if ( lines4intersect_validate( is_vertical(angle_avg), Point(x,y), Vec4i(x0,y0,x1,y1) ) ) {
// #ifndef ANDROID
    // cv::line ( pic, Point(x0, y0), Point(x1, y1), cv::Scalar(0,64,255), 2, CV_AA ); /*boost performance*/
// #endif // ANDROID
  }
}
