
// TODO - go on from here...
// http://docs.opencv.org/doc/tutorials/imgproc/imgtrans/hough_lines/hough_lines.html

// compile
// g++ $(pkg-config --cflags --libs opencv) canny_borders.cc -o canny_borders && ./canny_borders ./pics/heb.jpg

// outputs to ./img_pre/c1.jpg

// Fred Weinhaus image ready
// http://www.fmwconcepts.com/imagemagick/textcleaner/index.php
// ./textcleaner -g -e none -f 100 -o 3 pics/heb.jpg ./img_pre/heb.jpg

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

/// Global variables

Mat src, src_gray;
Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
char* window_name = "Edge Map";

std::vector<cv::Rect> detectLetters(cv::Mat img)
{
    // cv::namedWindow("Display Image", cv::WINDOW_NORMAL );

    std::vector<cv::Rect> boundRect;
    cv::Mat element, img_morph;

    element = getStructuringElement(cv::MORPH_RECT, cv::Size(17, 3) );
    cv::morphologyEx(img, img_morph, CV_MOP_CLOSE, element); //Does the trick

    cv::cvtColor(img_morph, img_morph, CV_RGB2GRAY);

    std::vector< std::vector< cv::Point> > contours;
    cv::findContours(img_morph, contours, 0, 1);
    std::vector<std::vector<cv::Point> > contours_poly( contours.size() );

    for( int i = 0; i < contours.size(); ++i )

      if (contours[i].size()>100) /*what's this shit 100?*/
        {
            cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
            cv::Rect appRect( boundingRect( cv::Mat(contours_poly[i]) ));
            if (appRect.width>appRect.height)
                boundRect.push_back(appRect);
        }
    return boundRect;
}

/**
 * @function CannyThreshold
 * @brief Trackbar callback - Canny thresholds input with a ratio 1:3
 */
void CannyThreshold(int, void*)
{
  /// Reduce noise with a kernel 3x3
  blur( src_gray, detected_edges, Size(3,3) );

  lowThreshold = 50;

  /// Canny detector
  Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );

  /// Using Canny's output as a mask, we display our result
  dst = Scalar::all(0);

  src.copyTo( dst, detected_edges);
  // imshow( window_name, dst );
  // cv::imwrite( "./img_pre/c0.jpg", dst);

  //Detect
  std::vector<cv::Rect> letterBBoxes1=detectLetters(dst);

  for ( int i=0; i< letterBBoxes1.size(); i++ ) {

      // DEBUG - un-mark for debug
      cv::rectangle(dst,letterBBoxes1[i],cv::Scalar(0,255,0),3,8,0);
  }

  cv::imwrite( "./img_pre/c1.jpg", dst);

 }

/** @function main */
int main( int argc, char** argv )
{
  /// Load an image
  src = imread( argv[1] );

  if( !src.data )
  { return -1; }

  /// Create a matrix of the same type and size as src (for dst)
  dst.create( src.size(), src.type() );

  /// Convert the image to grayscale
  cvtColor( src, src_gray, CV_BGR2GRAY );

  /// Create a window
  // namedWindow( window_name, CV_WINDOW_AUTOSIZE );

  /// Create a Trackbar for user to enter threshold
  // createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold );

  /// Show the image
  CannyThreshold(0, 0);

  /// Wait until user exit program by pressing a key
  // waitKey(0);

  return 0;
  }
