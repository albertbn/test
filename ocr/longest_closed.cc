
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
   // Mat mat = imread( "./pics/heb.jpg");
   Mat mat = imread( "./pics/heb2.jpg");
   // Mat mat = imread( "./pics/heb_new.jpg");
   // Mat mat = imread( "./pics/pers.jpg");
   // Mat mat = imread( "./pics/heb.ocv.working.jpg");
   // Mat mat = imread( "./pics/tj.jpg");
   // Mat mat = imread( "./pics/tj2.jpg");

   cv::cvtColor(mat, mat, CV_BGR2GRAY);
   // cv::GaussianBlur(mat, mat, cv::Size(3,3), 0);
   // blur(mat, mat, Size(20,20));
   cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Point(19,19));
   cv::Mat dilated;
   cv::dilate(mat, dilated, kernel);
   blur(dilated, dilated, Size(10,10));

   cv::imwrite( "./img_pre/long0.jpg", dilated );

   cv::Mat edges;
   cv::Canny(dilated, edges, 40, 1);
   blur(edges, edges, Size(10,10));

   cv::imwrite( "./img_pre/long1.jpg", edges);

   std::vector< std::vector<cv::Point> > contours;
   // cv::findContours(edges, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
   cv::findContours(edges, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS);

   std::cout << "contours count: " <<  contours.size()  << std::endl;

   // TODO - go on from checking if the >10000 is a single len
   double len;
   std::vector< std::vector<cv::Point> > contours_f1;
   std::vector<std::vector<cv::Point> > contoursDraw (contours.size());
   Mat poly = Mat::zeros( mat.size(), CV_8UC3 );
   for (int i=0; i < contours.size(); i++){

     cv::approxPolyDP(Mat(contours[i]), contoursDraw[i], 40, true);

     len = cv::arcLength(contours[i], true);

     if(len>0){
       std::cout << "closed line len...: " << len << std::endl;
       contours_f1.push_back(contours[i]);
     }
   }

   Mat drawing = Mat::zeros( mat.size(), CV_8UC3 );
   cv::drawContours(drawing, contours_f1, -1, cv::Scalar(0,255,0),1);
   cv::drawContours(poly, contoursDraw, -1, cv::Scalar(0,255,0),1);

   cv::imwrite( "./img_pre/long2.jpg", drawing);
   cv::imwrite( "./img_pre/long3.jpg", poly);
}

int main ( int argc, char** argv )
{
  longest_closed();
  return 0;
}
