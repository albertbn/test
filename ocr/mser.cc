/*
  g++ $(pkg-config --cflags --libs opencv) mser.cc -o mser && ./mser
*/

#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <math.h>

#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
  Mat mat = imread("./pics/heb2.jpg");

  cv::cvtColor(mat, mat, CV_BGR2GRAY);
  cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Point(19,19));
  cv::Mat dilated;
  cv::dilate(mat, dilated, kernel);
  blur(dilated, dilated, Size(10,10));

  MSER ms;
  Point p;
  vector< vector<Point> > regions;
  ms(dilated, regions, Mat());

  std::cout << "regions count... " << regions.size() << std::endl;

  for (int i = 0; i < regions.size(); i++)
    {
      ellipse(dilated, fitEllipse(regions[i]), Scalar(0,0,255));
    }

  cv::imwrite( "./img_pre/mser.jpg", dilated);
  return 0;
}
