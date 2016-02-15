/*
  g++ $(pkg-config --cflags --libs opencv) colour.cc -o colour && ./colour img/pic.jpg
*/

//http://opencv-srf.blogspot.co.il/2010/09/object-detection-using-color-seperation.html?m=1
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;

// int iLowH = 0;
// int iHighH = 179;
// int iLowS = 0;
// int iHighS = 255;
// int iLowV = 0;
// int iHighV = 255;

// HUE, SATURATION and VALUE
int iLowH = 10;
int iHighH = 200;
int iLowS = 10;
int iHighS = 200;
int iLowV = 10;
int iHighV = 200;

int main ( int argc, char** argv ) {

  if ( argc < 2  ) {
    cout << "please pass an image" << endl;
    return 1;
  }

  cout << "colour processing img.. " << argv[1] << endl;

  Mat imgOriginal = imread(argv[1]);

  Mat imgHSV;
  cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

  imwrite( "../img/scrap/imgHSV.jpg", imgHSV);

  Mat imgThresholded;
  inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

  //morphological opening (remove small objects from the foreground)
  erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
  dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

   //morphological closing (fill small holes in the foreground)
  dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
  erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );


  imwrite( "../img/scrap/imgThresholded.jpg", imgThresholded);

  return 0;
}
