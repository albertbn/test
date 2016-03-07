
// g++ -g -rdynamic $(pkg-config --cflags --libs opencv)  -o colour Object.cpp multipleObjectTracking.cpp

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>

#include "Object.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

vector < vector<Point> > contours_poly2; /*this is a static filed, that could be accessed from outside???*/
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS=50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20*20;



static double angle ( Point pt1, Point pt2, Point pt0 ) {

  double dx1 = pt1.x - pt0.x;
  double dy1 = pt1.y - pt0.y;
  double dx2 = pt2.x - pt0.x;
  double dy2 = pt2.y - pt0.y;
  double cos_value =
    (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);

  return acos(cos_value) * 180.0 / CV_PI;
}

// returns the angle90 count as well as draws circles... love u emacs
int get_angle_approx90_count ( std::vector<cv::Point> approx, Mat drawing, std::vector<cv::Point>& circles /*points4*/  ) {

  // Number of vertices of polygonal curve
  int vtc = approx.size();
  // std::cout << "vtc: " << vtc << std::endl;

  // DONE - go on and crack the cos/degree thing...
  // Get the degree (in cosines) of all corners
  // std::vector<double> cos;
  double ang, ang_deg;
  int angle90_count = 0;

  int j_mid;
  for ( int j = 1; j < vtc+1; j++ ) {

    (j==1 && (j_mid = vtc-1)) || (j_mid=j-2); /*6 nov 2015, Albert, Shawn 1 month old - fixed net/github script - go figure how come it's an educated world of assholes, writing un-perfect scripts */
    // std::cout << "approx indexes: " << j%vtc << ',' << j_mid << ',' << j-1 << std::endl;
    ang = ang_deg = angle(approx[j%vtc], approx[j_mid], approx[j-1]);
    // cos.push_back(ang);
    // ang_deg = abs(ang*180/CV_PI);

    if ( ang_deg >60.0 && ang_deg<120.0 ) {
      // cv::circle( drawing, approx[j%vtc], 50,  cv::Scalar(0,0,255) );
      ++angle90_count;
      circles.push_back(approx[j-1]);
      // std::cout << "drawing circles... "  << std::endl;
    }

    // std::cout << "angle is: " << ang_deg << ", " << ang  << std::endl;
  }

  double diag = 0;
  angle90_count && (diag = get_longest_side_poly ( circles ));

  if ( angle90_count ) {
    if(diag>100){
      filter_points_if_needed(circles, approx);
      angle90_count = circles.size();
      // std::cout << "OK, drawing circles... " << clen << std::endl;
      for ( int j=0; j<angle90_count; ++j ) {
        cv::circle( drawing, circles[j], 50,  cv::Scalar(50,0,255) );
      }
    }
  }

  return angle90_count;
}

//==============

void morphOps ( Mat &thresh ) {

  //create structuring element that will be used to "dilate" and "erode" image.
  //the element chosen here is a 3px by 3px rectangle
  Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
  //dilate with larger element so make sure object is nicely visible
  Mat dilateElement = getStructuringElement( MORPH_RECT,Size(8,8));

  erode(thresh,thresh,erodeElement);
  erode(thresh,thresh,erodeElement);

  dilate(thresh,thresh,dilateElement);
  dilate(thresh,thresh,dilateElement);
}

void trackFilteredObject ( Mat threshold, Mat &cameraFeed ) {

  vector <Object> objects;
  Mat temp;
  threshold.copyTo ( temp ) ;
  vector< vector<Point> > contours;
  contours_poly2.clear();

  vector<Vec4i> hierarchy;
  findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
  vector < vector<Point> > contours_poly(contours.size());
  if ( hierarchy.size() > 0 ) {

    int numObjects = hierarchy.size();
    //if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
    if ( numObjects>MAX_NUM_OBJECTS ) return;

    for (int index = 0; index >= 0; index = hierarchy[index][0]) {

      Moments moment = moments((cv::Mat)contours[index]);
      double area = moment.m00;

      if(area>MIN_OBJECT_AREA){
        // here - go to moon - yep indeed
        approxPolyDP ( Mat(contours[index]), contours_poly[index], 40, true );
        contours_poly2.push_back(contours_poly[index]);
      }
    }

    // credits: thanks PowHu for alpha 255, http://stackoverflow.com/questions/15916751/cvscalar-not-displaying-expected-color
    drawContours ( cameraFeed, contours_poly2, -1, Scalar(94,206,165,255), 5 ) ;
  }
}

// should modify the taken picture as a mat and eventually get to the OCR
void save_middle_class ( Mat picture ) {
  drawContours ( picture, contours_poly2, -1, Scalar(94,206,165,255), 5 ) ;
}

void do_frame ( Mat cameraFeed ) {

  Mat threshold;
  Mat HSV;

  Object white("white");
  //white
  // opencv cvtColor: http://docs.opencv.org/2.4/modules/imgproc/doc/miscellaneous_transformations.html
  // test and know
  cvtColor(cameraFeed,HSV,COLOR_BGR2HSV); /*not clear if image comes rgb(a) or bgr from java - bgr it is, yep!*/
  // cvtColor(cameraFeed,HSV,CV_RGB2HSV); /*not clear if image comes rgb(a) or bgr from java*/

  inRange(HSV,white.getHSVmin(),white.getHSVmax(),threshold);
  morphOps(threshold);
  trackFilteredObject ( threshold, cameraFeed );
}
