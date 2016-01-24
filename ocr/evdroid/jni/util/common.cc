
#include <iostream>
#include <ctime>

#include <sys/stat.h>
#include <opencv2/core/core.hpp>
#include "static_fields.hpp"

using namespace cv;
using namespace std;

float clock_ticks_to_ms ( unsigned int ticks ) {
  return ((float)ticks) / (CLOCKS_PER_SEC/1000);
}

bool file_exists ( const string& name ) {

  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

double get_max_deviation ( Size size, double angle_center, bool is_vert ) {

  double max_deviation=0.0, len_side, angle_center_rad;
  len_side = is_vert ? size.height : size.width;
  angle_center = is_vert ? angle_center-90.0 : 180-angle_center;
  angle_center_rad = angle_center/180*CV_PI;
  max_deviation = abs ( tan(angle_center_rad) * len_side ) ;

  return max_deviation;
}

void reduce_noise_short_lines ( std::vector < std::vector<cv::Point> > &contours, Mat_<float> &angles, std::vector<double> len_contours ) {

  Mat m ( len_contours );
  cv::Scalar mean, stdev;
  cv::meanStdDev(m, mean, stdev);

  std::vector < std::vector<cv::Point> > contours2;
  Mat_<float> angles2;
  std::vector<double>::iterator biggest = std::max_element(len_contours.begin(), len_contours.end());
  double d_stdev = stdev[0] / (*biggest / stdev[0]);

  float len_total = 0;

  for(int i=0; i<(int)len_contours.size(); ++i){
    if(len_contours[i]>=d_stdev){
      contours2.push_back(contours[i]);
      angles2.push_back(angles(0,i));
      len_total+=len_contours[i];
    }
  }

  if ( len_total < MIN_LINE_LENGTH_CONSIDERED_SIDE )
    contours2.clear();

  contours = contours2; angles = angles2; /*TODO - go on from here */
}
