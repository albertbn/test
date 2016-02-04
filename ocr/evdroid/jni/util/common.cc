
#include <iostream>
#include <ctime>

#include <sys/stat.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp> /*holds the INTER_CUBIC enum for resize*/
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
void reduce_noise_short_lines (
                               std::vector < std::vector<cv::Point> > &contours,
                               Mat_<float> &angles,
                               std::vector<double> len_contours
                               ) {

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

  contours = contours2; angles = angles2;
}

void reduce_noise_short_lines_raw_from_polydp (
                               std::vector < std::vector<cv::Point> > &contours,
                               std::vector<double> &len_contours
                               ) {

  // cout << "reduce_noise_short_lines :: contours" << Mat(contours) << endl;
  cout << "\n===\n\reduce_noise_short_lines_raw_from_polydp :: len_countours" << Mat(len_contours) << endl;
  Mat m ( len_contours );
  cv::Scalar mean, stdev;
  cv::meanStdDev(m, mean, stdev);

  std::vector < std::vector<cv::Point> > contours2;
  std::vector<double>::iterator longest = std::max_element(len_contours.begin(), len_contours.end());
  // double d_stdev = stdev[0] / (*longest / stdev[0]);
  double d_mean = mean[0]; !d_mean && (d_mean = .001);
  double d_stdev = stdev[0]; !d_stdev && (d_stdev=.001);
  cout << "\nreduce_noise_short_lines_raw_from_polydp :: d_mean, d_stdev, x/y, longest: " << d_mean << ',' << d_stdev << ',' << (d_mean/d_stdev) << ',' << *longest << endl;

  std::vector<double> len_contours2; /* NOT :) just for dump*/

  cout << "\n MIN_LINE_LENGTH_CONSIDERED_SIDE: " << MIN_LINE_LENGTH_CONSIDERED_SIDE << endl;
  cout << "\nreduce_noise_short_lines_raw_from_polydp :: mean, stdev: " << mean << ',' << stdev << endl;
  cout << "\nreduce_noise_short_lines_raw_from_polydp :: d_stdev: " << d_stdev  << endl;

  // contours2.clear(); len_contours2.clear();
  for(int i=0; i<(int)contours.size(); ++i){
    if ( len_contours[i]>= MIN_LINE_LENGTH_CONSIDERED_SIDE ){
      contours2.push_back(contours[i]);
      len_contours2.push_back(len_contours[i]);
    }
  }

  cout << "\nreduce_noise_short_lines_raw_from_polydp :: final len_contours2" << Mat(len_contours2)  << endl;
  contours = contours2; len_contours = len_contours2;
}

// @contoursDraw3 - append also, don't re-set
// @len_contours_contoursDraw - this vector should be filled in, NOT re-set - called from split_contours_2_dotted_lines (in main_geometry.cc), yep? I'LL BE BACK!
void split_lines_analyze_n_reduce_background_noise (
                                              std::vector<cv::Point> line_poly,
                                              std::vector<std::vector<cv::Point> > &contoursDraw3,
                                              std::vector<double> &len_contours_contoursDraw
                                              ) {

  std::vector<cv::Point> line_tmp;
  std::vector<double> len_contours;
  double len;
  int ssize = (int)line_poly.size();
  std::vector < std::vector<cv::Point> > contours;
  for ( int i=0; i<ssize-1; ++i ) { /*don't do last with first - that is, don't plot a closing line */
    line_tmp.clear ( );
    line_tmp.push_back ( line_poly[i] );
    line_tmp.push_back ( line_poly[i+1] );
    len = cv::arcLength ( line_tmp, true );
    len_contours.push_back(len);
    contours.push_back(line_tmp);
  }

  // ok got the line lengths
  // two choices I have - one is to create a logic with neighbor short lines
  // the other one is a standard deviation - get longest

  // trying first the standard deviation...
  // try using the existing reduce reduce_noise_short_lines
  reduce_noise_short_lines_raw_from_polydp ( contours /*ref*/, len_contours /*ref*/ );
  //concatenate vector
  //credits: http://stackoverflow.com/questions/2551775/c-appending-a-vector-to-a-vector
  contoursDraw3.insert(contoursDraw3.end(), contours.begin(), contours.end());
  len_contours_contoursDraw.insert(len_contours_contoursDraw.end(), len_contours.begin(), len_contours.end());
}

// down scales if needed
void mat_downscale_check ( Mat &mat ) {

  int large = max(mat.size().width, mat.size().height);
  if ( large<5500 ) return;

  resize ( mat, mat, Size(), .75, .75, INTER_CUBIC );
}

//~/Desktop/fail/vip.jpg - fixed
//evdroid4.jpg


// void reduce_noise_short_lines (
//                                std::vector < std::vector<cv::Point> > &contours,
//                                Mat_<float> &angles,
//                                std::vector<double> &len_contours
//                                ) {

//   // cout << "reduce_noise_short_lines :: contours" << Mat(contours) << endl;
//   // if(angles.size().width>0)
//   //   cout << "reduce_noise_short_lines :: angles" << angles << endl;
//   cout << "\n===\n\nreduce_noise_short_lines :: len_countours" << Mat(len_contours) << endl;
//   Mat m ( len_contours );
//   cv::Scalar mean, stdev;
//   cv::meanStdDev(m, mean, stdev);

//   std::vector < std::vector<cv::Point> > contours2;
//   Mat_<float> angles2;
//   std::vector<double>::iterator longest = std::max_element(len_contours.begin(), len_contours.end());
//   // double d_stdev = stdev[0] / (*longest / stdev[0]);
//   double d_mean = mean[0]; !d_mean && (d_mean = .001);
//   double d_stdev = stdev[0]; !d_stdev && (d_stdev=.001);
//   cout << "\n d_mean, d_stdev, x/y, longest: " << d_mean << ',' << d_stdev << ',' << (d_mean/d_stdev) << ',' << *longest << endl;

//   std::vector<double> len_contours2; /* NOT :) just for dump*/
//   //=========
//   // BUSTED - only short lines
//   //=========
//   if ( (d_mean/d_stdev)>4.5 && d_mean<(MIN_LINE_LENGTH_CONSIDERED_SIDE/2.5) ){
//     contours = contours2; angles = angles2;
//     return;
//   }

//   //=========
//   // this most probably means - just take the longest line and discard the rest
//   //=========
//   else if ( d_mean<(MIN_LINE_LENGTH_CONSIDERED_SIDE/2.5) && (*longest/d_mean)>2.5 ){

//     // credits:http://stackoverflow.com/questions/2152986/effective-way-to-get-the-index-of-an-iterator
//     int index = longest - len_contours.begin();
//     cout << "\n index of longest: " << index << endl;
//     contours2.push_back(contours[index]);
//     if(angles.size().width>0)
//       angles2.push_back(angles(0,index));
//     contours = contours2; angles = angles2;
//     len_contours2.push_back(*longest);
//     len_contours = len_contours2; /*NOT :) just for dump*/
//     return;
//   }

//   cout << "\n MIN_LINE_LENGTH_CONSIDERED_SIDE: " << MIN_LINE_LENGTH_CONSIDERED_SIDE << endl;
//   cout << "\nreduce_noise_short_lines :: mean, stdev: " << mean << ',' << stdev << endl;
//   cout << "\nreduce_noise_short_lines :: d_stdev: " << d_stdev  << endl;

//   float len_total = 0;

//   for ( int i=0; i<(int)len_contours.size(); ++i ) {
//     if(len_contours[i]>=d_stdev){
//       contours2.push_back(contours[i]);
//       if(angles.size().width>0)
//         angles2.push_back(angles(0,i));
//       len_total+=len_contours[i];
//       len_contours2.push_back(len_contours[i]); /* NOT :)just for dump */
//     }
//   }

//   if ( len_total < MIN_LINE_LENGTH_CONSIDERED_SIDE ) {
//     contours2.clear(); len_contours2.clear();
//   }

//   cout << "\nreduce_noise_short_lines :: final len_contours2" << Mat(len_contours2)  << endl;
//   contours = contours2; angles = angles2; len_contours2 = len_contours;
// }

