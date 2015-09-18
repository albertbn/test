
// g++ $(pkg-config --cflags --libs opencv) corners.cc -o corners && ./corners

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <math.h>

#include <iostream>

using namespace cv;
using namespace std;

// http://stackoverflow.com/questions/6555629/algorithm-to-detect-corners-of-paper-sheet-in-photo
void corners()
{
   Mat mat = imread( "./pics/heb.jpg");

   cv::cvtColor(mat, mat, CV_BGR2GRAY);
   cv::GaussianBlur(mat, mat, cv::Size(3,3), 0);
   cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Point(9,9));
   cv::Mat dilated;
   cv::dilate(mat, dilated, kernel);

   cv::Mat edges;
   cv::Canny(dilated, edges, 84, 3);

   std::vector<cv::Vec4i> lines;
   lines.clear();
   cv::HoughLinesP(edges, lines, 1, CV_PI/180, 25);
   std::vector<cv::Vec4i>::iterator it = lines.begin();
   for(; it!=lines.end(); ++it) {
     cv::Vec4i l = *it;
     cv::line(edges, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255,0,0), 2, 8);
   }
   std::vector< std::vector<cv::Point> > contours;
   cv::findContours(edges, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS);
   std::vector< std::vector<cv::Point> > contoursCleaned;
   for (int i=0; i < contours.size(); i++) {
     if (cv::arcLength(contours[i], false) > 100)
       contoursCleaned.push_back(contours[i]);
   }
   std::vector<std::vector<cv::Point> > contoursArea;

   for (int i=0; i < contoursCleaned.size(); i++) {
     if (cv::contourArea(contoursCleaned[i]) > 10000){
       contoursArea.push_back(contoursCleaned[i]);
     }
   }
   std::vector<std::vector<cv::Point> > contoursDraw (contoursCleaned.size());
   for (int i=0; i < contoursArea.size(); i++){
     cv::approxPolyDP(Mat(contoursArea[i]), contoursDraw[i], 40, true);
   }
   Mat drawing = Mat::zeros( mat.size(), CV_8UC3 );
   cv::drawContours(drawing, contoursDraw, -1, cv::Scalar(0,255,0),1);

   // namedWindow( "fock", CV_WINDOW_AUTOSIZE );
   // imshow( "fock", drawing );
   // waitKey(0);

   cv::imwrite( "./img_pre/cor0.jpg", drawing);
}

int main ( int argc, char** argv )
{
  corners();
  return 0;
}
