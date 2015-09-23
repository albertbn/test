
// this outputs a black with a 3 of 4 lines of the tilted invoice - from stack, the python
// translation to c++...

// g++ $(pkg-config --cflags --libs opencv) corners.cc -o corners && ./corners

#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <math.h>

#include <iostream>

using namespace cv;
using namespace std;

// http://stackoverflow.com/questions/6555629/algorithm-to-detect-corners-of-paper-sheet-in-photo

/**
 * Helper function to find a cosine of angle between vectors
 * from pt0->pt1 and pt0->pt2
 https://github.com/bsdnoobz/opencv-code/blob/master/shape-detect.cpp
 http://opencv-code.com/tutorials/detecting-simple-shapes-in-an-image/
 */
static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

// this is not what i need... i need just a simple diagonal - tl, br...
int get_longest_side_poly_magn( std::vector<cv::Point> approx ){

  int len = approx.size();

  Mat xs = Mat_<double>(approx.size(),CV_32F),
    ys = Mat_<double>(approx.size(),CV_32F);

  for(int i=0; i<len; ++i){
    xs.at<double>(i) = approx[i].x;
    ys.at<double>(i) = approx[i].y;
  }

  cv::Mat magns(xs.size(), xs.type());

  magnitude(xs, ys, magns);
  double min, max;
  cv::Point min_loc, max_loc;
  minMaxLoc ( magns, &min, &max, &min_loc, &max_loc );
  std::cout << "min max magns are: " << min << ',' <<  max << ',' << max_loc << std::endl;

  return 1;
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
  for (int j = 2; j < vtc+1; j++) {

    ang = angle(approx[j%vtc], approx[j-2], approx[j-1]);
    // cos.push_back(ang);

    ang_deg = ang*180/CV_PI;

    if(ang_deg >-10 && ang_deg<10){
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

void corners()
{
   // Mat mat = imread( "./pics/heb.jpg");
   // Mat mat = imread( "./pics/heb_new.jpg");
   Mat mat = imread( "./pics/heb2.jpg");

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

   std::cout << "lines count: " << lines.size()  << std::endl;

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

   std::cout << "contoursCleaned count: " << contoursCleaned.size()  << std::endl;

   std::vector<std::vector<cv::Point> > contoursArea;
   for (int i=0; i < contoursCleaned.size(); i++) {
     if (cv::contourArea(contoursCleaned[i]) > 5000){
       contoursArea.push_back(contoursCleaned[i]);
       std::cout << "carea points count: " << contoursCleaned[i].size() << std::endl;
     }
   }

   std::cout << "contoursArea count (this is where the animal stinks): " <<  contoursArea.size()  << std::endl;

   std::vector<std::vector<cv::Point> > contoursDraw (contoursCleaned.size());
   Mat drawing = Mat::zeros( mat.size(), CV_8UC3 );
   for (int i=0; i < contoursArea.size(); i++){
     cv::approxPolyDP(Mat(contoursArea[i]), contoursDraw[i], 40, true);

     std::cout << "draw lines count: " << contoursDraw[i].size() << std::endl;

     // (i==4 || i==8) && get_angles( contoursDraw[i], drawing );
     get_angles( contoursDraw[i], drawing );

     // std::vector<std::vector<cv::Point> > ctemp;
     // ctemp.push_back(contoursDraw[i]);
     // cv::drawContours(drawing, ctemp, -1, cv::Scalar(0 , 0, 255),1);
     // std::ostringstream filename;
     // filename <<  "./img_pre/cor0" << i << ".jpg";
     // cv::imwrite( filename.str(), drawing);
   }
   // Mat drawing = Mat::zeros( mat.size(), CV_8UC3 );
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
