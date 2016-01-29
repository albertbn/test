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

bool directory_exists ( string path ) {
   struct stat statbuf;

   if ( stat(path.c_str(), &statbuf) == -1 )
      return 0;
   else
      return S_ISDIR(statbuf.st_mode);
}

double get_max_deviation ( Size size, double angle_center, bool is_vert ) {

  double max_deviation=0.0, len_side, angle_center_rad;
  len_side = is_vert ? size.height : size.width;
  angle_center = is_vert ? angle_center-90.0 : 180-angle_center;
  angle_center_rad = angle_center/180*CV_PI;
  max_deviation = abs ( tan(angle_center_rad) * len_side ) ;

  return max_deviation;
}

// standard deviation
void reduce_noise_short_lines ( std::vector < std::vector<cv::Point> > &contours, Mat_<float> &angles, std::vector<double> len_contours ) {

  // cout << "reduce_noise_short_lines :: contours" << Mat(contours) << endl;
  // cout << "reduce_noise_short_lines :: angles" << angles << endl;
  cout << "\n\n===\n\nreduce_noise_short_lines :: len_countours" << Mat(len_contours) << endl;

  Mat m ( len_contours );
  cv::Scalar mean, stdev;
  cv::meanStdDev(m, mean, stdev);

  std::vector < std::vector<cv::Point> > contours2;
  Mat_<float> angles2;
  std::vector<double>::iterator longest = std::max_element(len_contours.begin(), len_contours.end());
  // double d_stdev = stdev[0] / (*longest / stdev[0]);
  double d_mean = mean[0]; !d_mean && (d_mean = .001);
  double d_stdev = stdev[0]; !d_stdev && (d_stdev=.001);
  cout << "\n d_mean, d_stdev, x/y, longest: " << d_mean << ',' << d_stdev << ',' << (d_mean/d_stdev) << ',' << *longest << endl;
  if ( (d_mean/d_stdev)>4.5 && d_mean<(MIN_LINE_LENGTH_CONSIDERED_SIDE/2.5) ){
    contours = contours2; angles = angles2;
    return;
  }
  else if ( d_mean<(MIN_LINE_LENGTH_CONSIDERED_SIDE/2.5) && (*longest/d_mean)>2.5 ){
    int index = longest - len_contours.begin();
    cout << "\nindex of longest: " << index << endl;
    contours2.push_back(contours[index]);
    angles2.push_back(angles(0,index));
    contours = contours2; angles = angles2;
    return;
  }

  cout << "\n MIN_LINE_LENGTH_CONSIDERED_SIDE: " << MIN_LINE_LENGTH_CONSIDERED_SIDE << endl;
  cout << "\nreduce_noise_short_lines :: mean, stdev: " << mean << ',' << stdev << endl;
  cout << "\nreduce_noise_short_lines :: d_stdev: " << d_stdev  << endl;

  float len_total = 0;
  std::vector<double> len_contours2; /*just for dump*/

  for ( int i=0; i<(int)len_contours.size(); ++i ) {
    if(len_contours[i]>=d_stdev){
      contours2.push_back(contours[i]);
      angles2.push_back(angles(0,i));
      len_total+=len_contours[i];
      len_contours2.push_back(len_contours[i]);/*just for dump*/
    }
  }

  if ( len_total < MIN_LINE_LENGTH_CONSIDERED_SIDE )
    contours2.clear();

  cout << "\nreduce_noise_short_lines :: final len_contours2" << Mat(len_contours2)  << endl;

  contours = contours2; angles = angles2;
}
