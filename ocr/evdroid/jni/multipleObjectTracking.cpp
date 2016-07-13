// g++ -g -rdynamic $(pkg-config --cflags --libs opencv)  -o colour multipleObjectTracking.cpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "static_fields.hpp"
#include "ocr/tess.hpp" /*used for rotate - rot90, init_ocr*/
#include "ocr/main.hpp"

const float width_small = 640.0 ;
const float height_small = 480.0;
// const float width = 3264.0;
// const height = 2448.0;
const float width = 2048.0 ;
const float height = 1536.0;

const float x_ratio = width/width_small;
const float y_ratio = height/height_small;

Point center;

//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS=50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20*20;
// const int MIN_OBJECT_AREA = 5*5;

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
int get_angle_approx90_count ( vector<Point> approx, vector<Point>& circles /*points4*/  ) {

  // Number of vertexes of polygonal curve
  int vtc = approx.size();

  double ang;
  int angle90_count = 0;

  int j_mid;
  for ( int j = 1; j < vtc+1; j++ ) {

    (j==1 && (j_mid = vtc-1)) || (j_mid=j-2);
    ang = angle(approx[j%vtc], approx[j_mid], approx[j-1]);

    if ( ang >60.0 && ang < 120.0 ) {
      // circle( drawing, approx[j%vtc], 50,  Scalar(0,0,255) );
      ++angle90_count;
      circles.push_back(approx[j-1]);
    }
  }

  return angle90_count;
}

//==============

void morphOps ( Mat& thresh ) {

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

void trackFilteredObject ( Mat& threshold ) {

  Mat temp;
  threshold.copyTo ( temp ) ;
  vector< vector<Point> > contours;
  contours_poly2.clear();

  vector<Vec4i> hierarchy;
  findContours ( temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
  vector < vector<Point> > contours_poly(contours.size());
  if ( hierarchy.size() > 0 ) {

    int numObjects = hierarchy.size();
    //if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
    if ( numObjects>MAX_NUM_OBJECTS ) return;

    for (int index = 0; index >= 0; index = hierarchy[index][0]) {

      Moments moment = moments((Mat)contours[index]);
      double area = moment.m00;

      if ( area>MIN_OBJECT_AREA ) {
        // here - go to moon - yep indeed
        approxPolyDP ( Mat(contours[index]), contours_poly[index], 40, true );
        contours_poly2.push_back(contours_poly[index]);
      }
    }

    // credits: thanks PowHu for alpha 255, http://stackoverflow.com/questions/15916751/cvscalar-not-displaying-expected-color
    // drawContours ( cameraFeed, contours_poly2, -1, Scalar(94,206,165,255), 5 ) ;
  }
}

void relocate_poly ( vector<Point> &points4 ) {

  int x,y;
  for ( int i=0; i<(int)points4.size(); ++i ) {
    x = points4[i].x; y = points4[i].y;
    x*=x_ratio; y*=y_ratio;
    points4[i] = Point(x,y);
  }
}

bool sortCorners ( vector<Point>& corners, Point center ) {

  bool ret = false;
  vector<Point> top, bot;

  for (int i = 0; i < (int)corners.size(); i++) {
    if (corners[i].y < center.y)
      top.push_back(corners[i]);
    else
      bot.push_back(corners[i]);
  }

  if ( top.size() == 2 && bot.size() == 2 ) {

    corners.clear();
    ret = true;

    Point tl = top[0].x > top[1].x ? top[1] : top[0];
    Point tr = top[0].x > top[1].x ? top[0] : top[1];
    Point bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
    Point br = bot[0].x > bot[1].x ? bot[0] : bot[1];

    corners.push_back(tl);
    corners.push_back(tr);
    corners.push_back(br);
    corners.push_back(bl);
  }

  return ret;
}

bool corners_magick_do ( vector<Point>& corners /*points4*/ ) {

  bool are4pointsfine = false;
  center = Point(0,0); /*yep, also stoned solving :) yep!*/
  // Get mass center
  for ( int i = 0; i < (int)corners.size(); ++i )
    center += corners[i];
  center *= ( 1. / corners.size() );

  are4pointsfine = sortCorners ( corners, center );
  return are4pointsfine;
}

void final_magic_crop_rotate ( Mat& mat, vector<Point>& points4 ) {

  Size size_mat = mat.size();
  corners_magick_do ( points4 /*ref*/ ); /*sorts corner points4*/

  vector<Point2f> points4f;
  // this here is probably closest to the size of the original invoice... well, let's try... tension :)
  RotatedRect rect_minAreaRect = minAreaRect ( points4 );

  // RNG rng(12345);
  Point2f rect_points[4]; rect_minAreaRect.points( rect_points );

  for ( int i=0; i<(int)points4.size(/*4*/); ++i ) {
    points4f.push_back(points4[i]);
  }

  bool is_mat_width = size_mat.width>size_mat.height; /*is width larger*/
  int small = min(rect_minAreaRect.size.width, rect_minAreaRect.size.height);
  int large = max(rect_minAreaRect.size.width, rect_minAreaRect.size.height);
  !is_mat_width && (small=small^large) && (large=small^large) && (small=small^large); /*XOR swap*/
  // Mat quad = Mat::zeros ( small, large, CV_8UC3 );
  Mat quad = Mat::zeros ( small, large, CV_8U );

  vector<Point2f> quad_pts;
  quad_pts.push_back(Point2f(0, 0));
  quad_pts.push_back(Point2f(quad.cols, 0));
  quad_pts.push_back(Point2f(quad.cols, quad.rows));
  quad_pts.push_back(Point2f(0, quad.rows));

  if ( points4f.size()==4 ) {
    outfile << "ok, doing pers transform and warp..." << points4f << endl;
    Mat transmtx = getPerspectiveTransform ( points4f, quad_pts );
    warpPerspective ( mat, quad, transmtx, quad.size() );

    imwrite ( IMG_PATH, quad ) ;
    ocr_doit ( quad );
  }
  else {
    outfile << "checking points4f... not 4 of number " << points4f << endl;
    // TODO rotate
    if ( mat.cols>mat.rows ){
      rot90 ( mat, 1 );
    }
    imwrite ( IMG_PATH, mat ) ;
    outfile << "DISPLAY_IMG" << endl;
  }
}

void test_ocr_pic_hardcoded ( ) {

  // outfile << "trying to read: " << IMG_PATH << endl;

  Mat mat_pic_hardcoded_smc = imread ( IMG_PATH, 0 ) ;
  ocr_doit ( mat_pic_hardcoded_smc );
}

// should modify the taken picture as a mat and eventually get to the OCR
void save_middle_class ( Mat& picture ) {

  // TEMP test the OCR with hard coded pic
  // test_ocr_pic_hardcoded();
  // return;

  cvtColor(picture, picture, CV_BGR2GRAY);

  int _angle90_count=0; vector<Point> points4;

  // count the ~90 degree angles...
  for ( int i=0; i<(int)contours_poly2.size(); ++i ) {
    relocate_poly( contours_poly2[i]);
    _angle90_count += get_angle_approx90_count ( contours_poly2[i], points4/*ref*/ );
    // points4 now should have the corners - go on with affine
  }
  drawContours ( picture, contours_poly2, -1, Scalar(94,206,165), 5 ) ;

  // TEMP
  // return;

  // getPerspectiveTransform, warpPerspective
  // final_magic_crop_rotate, corners_magick_do, sortCorners - good luck, may the force be with you
  final_magic_crop_rotate ( picture /*ref*/, points4 /*ref*/ );
}

// this folk is responsible for modifying the video preview (each img frame)
// by colour detecting and drawing a green frame
void do_frame ( Mat& cameraFeed, cv::Scalar hsv_min, cv::Scalar hsv_max ) {

  Mat HSV;
  Mat threshold;

  // Object white("white");
  //white
  // opencv cvtColor: http://docs.opencv.org/2.4/modules/imgproc/doc/miscellaneous_transformations.html
  // test and know
  cvtColor(cameraFeed, HSV, COLOR_BGR2HSV); /*not clear if image comes rgb(a) or bgr from java - bgr it is, yep!*/
  // cvtColor(cameraFeed,HSV,CV_RGB2HSV); /*not clear if image comes rgb(a) or bgr from java*/

  // inRange(HSV,white.getHSVmin(),white.getHSVmax(),threshold);
  inRange(HSV, hsv_min, hsv_max, threshold);
  morphOps(threshold);
  trackFilteredObject ( threshold );
}
