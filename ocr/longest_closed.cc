
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

static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

// get the diagonal of the bounding rect...
double get_longest_side_poly( std::vector<cv::Point> approx ){

  Rect rect = boundingRect(approx);

  return sqrt(rect.width*rect.width + rect.height*rect.height );
}

int get_angles ( std::vector<cv::Point> approx, Mat drawing ) {

  // Number of vertices of polygonal curve
  int vtc = approx.size();

  // Get the degree (in cosines) of all corners
  // std::vector<double> cos;
  double ang, ang_deg;
  int has_angle90 = 0;
  std::vector<cv::Point> circles;
  for (int j = 1; j < vtc+1; j++) {

    ang = angle(approx[j%vtc], approx[j-2], approx[j-1]);
    // cos.push_back(ang);

    ang_deg = ang*180/CV_PI;

    if(ang_deg >-25 && ang_deg<25){
      // cv::circle( drawing, approx[j%vtc], 50,  cv::Scalar(0,0,255) );
      has_angle90 = 1;
      circles.push_back(approx[j-1]);
      // std::cout << "drawing circles... "  << std::endl;
    }

    // std::cout << "angle is: " << ang_deg << ", " << ang  << std::endl;
  }

  double diag = 0;
  has_angle90 && (diag = get_longest_side_poly ( circles ));

  if(has_angle90){

    std::cout << "diag is: " << diag << std::endl;

    if(diag>100){
      std::cout << "ok, drawing circles... " << std::endl;
      int clen = circles.size();
      for(int j=0; j<clen; ++j){

        cv::circle( drawing, circles[j], 50,  cv::Scalar(50,0,255) );
      }
    }

  }

  return 0;
}


void longest_closed()
{
   Mat mat = imread( "./pics/heb.jpg");
   // Mat mat = imread( "./pics/heb2.jpg");
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

   /// Apply Histogram Equalization - not clear - sometimes is good, sometimes not???
   // equalizeHist ( dilated, dilated );

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
   std::vector< std::vector<cv::Point> > contours_long;
   std::vector<std::vector<cv::Point> > contoursDraw (contours.size());
   Mat poly = Mat::zeros( mat.size(), CV_8UC3 );
   for (int i=0; i < contours.size(); i++){

     cv::approxPolyDP(Mat(contours[i]), contoursDraw[i], 40, true);

     len = cv::arcLength(contours[i], true);

     if(len>0){
       std::cout << "closed line len...: " << len << std::endl;
       contours_f1.push_back(contours[i]);
       if(len>5000) contours_long.push_back(contoursDraw[i]);
     }
   }

   Mat drawing = Mat::zeros( mat.size(), CV_8UC3 );
   Mat clong = Mat::zeros( mat.size(), CV_8UC3 );
   cv::drawContours(drawing, contours_f1, -1, cv::Scalar(0,255,0),1);
   cv::drawContours(poly, contoursDraw, -1, cv::Scalar(0,255,0),1);
   cv::drawContours(clong, contours_long, -1, cv::Scalar(0,255,0),1);

   for(int i=0; i<contours_long.size(); ++i){
     get_angles( contours_long[i], clong );
   }

   cv::imwrite( "./img_pre/long2.jpg", drawing);
   cv::imwrite( "./img_pre/long3.jpg", poly);
   cv::imwrite( "./img_pre/long4.jpg", clong);
}

int main ( int argc, char** argv )
{
  longest_closed();
  return 0;
}
