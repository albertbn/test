

// g++ $(pkg-config --cflags --libs opencv) histogram.cc -o histogram && ./histogram ./img_pre/heb.jpg

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

/**  @function main */
int main( int argc, char** argv )
{
  Mat src, dst;

  /// Load image
  src = imread( argv[1], 1 );

  if( !src.data )
    { cout<<"Usage: ./histogram <path_to_image>" << endl;
      return -1;}

  /// Convert to grayscale
  cvtColor( src, src, CV_BGR2GRAY );

  /// Apply Histogram Equalization
  equalizeHist
    ( src, dst );

  cv::imwrite( "./img_pre/hist.jpg", dst);

  return 0;
}
