
// compile
// g++ $(pkg-config --cflags --libs opencv) hough.cc -o hough && ./hough ./pics/heb.jpg

// credits: http://docs.opencv.org/doc/tutorials/imgproc/imgtrans/hough_lines/hough_lines.html

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <math.h>

#include <iostream>

using namespace cv;
using namespace std;

void help()
{
 cout << "\nThis program demonstrates line finding with the Hough transform.\n"
         "Usage:\n"
         "./houghlines <image_name>, Default is pic1.jpg\n" << endl;
}

float get_line_len(Vec4i points){

  // use Pythagorean Theorem
  int a = points[3] -  points[1];
  int b = points[2] -  points[0];

  return sqrt(a*a + b*b);
}

float get_line_angle(Vec4i points){

  // use Pythagorean Theorem
  int a = points[3] -  points[1];
  int b = points[2] -  points[0];

  return atan2(a*a, b*b) *  180 / CV_PI;
}

int main(int argc, char** argv)
{
 const char* filename = argc >= 2 ? argv[1] : "pic1.jpg";

 Mat src = imread(filename, 0);
 if(src.empty())
 {
     help();
     cout << "can not open " << filename << endl;
     return -1;
 }

 Mat dst, cdst;
 Canny(src, dst, 50, 200, 3);
 cvtColor(dst, cdst, CV_GRAY2BGR);

 #if 0
  vector<Vec2f> lines;
  HoughLines(dst, lines, 1, CV_PI/180, 100, 0, 0 );

  for( size_t i = 0; i < lines.size(); i++ )
  {
     float rho = lines[i][0], theta = lines[i][1];
     Point pt1, pt2;
     double a = cos(theta), b = sin(theta);
     double x0 = a*rho, y0 = b*rho;
     pt1.x = cvRound(x0 + 1000*(-b));
     pt1.y = cvRound(y0 + 1000*(a));
     pt2.x = cvRound(x0 - 1000*(-b));
     pt2.y = cvRound(y0 - 1000*(a));
     line( cdst, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
  }
 #else
  vector<Vec4i> lines;
  HoughLinesP(dst, lines, 1, CV_PI/180, 50, 50, 10 );

  std::cout << "No. of lines: " << lines.size() << std::endl;

  float angle = 0., angle2=0.;
  for( size_t i = 0; i < lines.size(); i++ )
  {
    Vec4i l = lines[i];
    line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);

    angle2 = get_line_angle(l);
    angle+=angle2;

    std::cout << "line length is: " << get_line_len(l) << std::endl;
    std::cout << "line angle is: " << angle2 << std::endl;
  }

  std::cout << "rotation is: " << angle/lines.size() << std::endl;

 #endif
 // imshow("source", src);
 // imshow("detected lines", cdst);
  imwrite("./img_pre/hough.jpg", cdst);

 // waitKey();

 return 0;
}
