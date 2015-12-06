
// g++ $(pkg-config --cflags --libs opencv) lines.cc -o lines && ./lines

#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <math.h>
#include <iostream>
#include <sys/stat.h>
#include "opencv2/flann/flann.hpp"

using namespace cv;
using namespace std;

int main ( int argc, char** argv )
{

  Mat1f cluster_data(15,3);
  cluster_data(0,0) = 1;  cluster_data(0,1) = 1;cluster_data(0,2) = 1;
  cluster_data(1,0) = 2;  cluster_data(1,1) = 2;cluster_data(1,2) = 1;
  cluster_data(2,0) = 2;  cluster_data(2,1) = 1;cluster_data(2,2) = 1;
  cluster_data(3,0) = 1;  cluster_data(3,1) = 1;cluster_data(3,2) = 2;
  cluster_data(4,0) = 1;  cluster_data(4,1) = 2;cluster_data(4,2) = 2;

  cluster_data(5,0) = 100;    cluster_data(5,1) = 99;cluster_data(5,2) = 98;
  cluster_data(6,0) = 101;    cluster_data(6,1) = 100;cluster_data(6,2) = 99;
  cluster_data(7,0) = 103;    cluster_data(7,1) = 90;cluster_data(7,2) = 97;
  cluster_data(8,0) = 99; cluster_data(8,1) = 98;cluster_data(8,2) = 102;
  cluster_data(9,0) = 98; cluster_data(9,1) = 100;cluster_data(9,2) = 102;

  cluster_data(10,0) = 1001;  cluster_data(10,1) = 997;cluster_data(10,2) = 996;
  cluster_data(11,0) = 1002;  cluster_data(11,1) = 1001;cluster_data(11,2) = 999;
  cluster_data(12,0) = 1000;  cluster_data(12,1) = 999;cluster_data(12,2) = 998;
  cluster_data(13,0) = 999;   cluster_data(13,1) = 998;cluster_data(13,2) = 1000;
  cluster_data(14,0) = 998;   cluster_data(14,1) = 1003;cluster_data(14,2) = 1003;

  cvflann::KMeansIndexParams k_params(14, 1000, cvflann::FLANN_CENTERS_KMEANSPP,0.01);

  Mat1f centers(14,3);
  centers.setTo(0);

  int count = cv::flann::hierarchicalClustering<cv::flann::L2<float> >(cluster_data,centers,k_params);

  cout << "count: " << count << "\ncd:" << cluster_data <<"\ncenters: " << centers << endl;

  return 0;
}
