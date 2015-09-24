
// g++ $(pkg-config --cflags --libs opencv) longest_closed.cc -o longest_closed && ./longest_closed

#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <math.h>

#include <iostream>

using namespace cv;
using namespace std;

// http://stackoverflow.com/questions/6555629/algorithm-to-detect-longest_closed-of-paper-sheet-in-photo


void longest_closed()
{
   Mat mat = imread( "./pics/heb.jpg");

   cv::cvtColor(mat, mat, CV_BGR2GRAY);
   cv::GaussianBlur(mat, mat, cv::Size(3,3), 0);
   // blur(mat, mat, Size(3,3));
   cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Point(9,9));
   cv::Mat dilated;
   cv::dilate(mat, dilated, kernel);

   cv::Mat edges;
   cv::Canny(dilated, edges, 100, 3);

   cv::imwrite( "./img_pre/long0.jpg", edges);

   std::vector< std::vector<cv::Point> > contours;
   cv::findContours(edges, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

   std::cout << "contours count: " <<  contours.size()  << std::endl;

   double len;
   std::vector< std::vector<cv::Point> > contours_f1;
   for (int i=0; i < contours.size(); i++){

     len = cv::arcLength(contours[i], true);

     if(len>3000){
       std::cout << "closed line...: " << len << std::endl;
       contours_f1.push_back(contours[i]);
     }
   }

   Mat drawing = Mat::zeros( mat.size(), CV_8UC3 );
   cv::drawContours(drawing, contours_f1, -1, cv::Scalar(0,255,0),1);

   cv::imwrite( "./img_pre/long1.jpg", drawing);
}

int main ( int argc, char** argv )
{
  longest_closed();
  return 0;
}
