
// g++ $(pkg-config --cflags --libs opencv)opencv_rotate_heb.cc -oopencv_rotate_heb && ./opencv_rotate_heb

#include "opencv2/opencv.hpp"

// testing: http://felix.abecassis.me/2011/10/opencv-rotation-deskewing/
void deskew(const char* filename, double angle)
{
  cv::Mat img = cv::imread(filename, 0);

  cv::bitwise_not(img, img);

  std::vector<cv::Point> points;
  cv::Mat_<uchar>::iterator it = img.begin<uchar>();
  cv::Mat_<uchar>::iterator end = img.end<uchar>();
  for (; it != end; ++it)
    if (*it)
      points.push_back(it.pos());

  cv::RotatedRect box = cv::minAreaRect(cv::Mat(points));

  cv::Mat rot_mat = cv::getRotationMatrix2D(box.center, angle, 1);

  cv::Mat rotated;
  cv::warpAffine(img, rotated, rot_mat, img.size(), cv::INTER_CUBIC);

  cv::Size box_size = box.size;
  if (box.angle < -45.)
    std::swap(box_size.width, box_size.height);
  cv::Mat cropped;
  cv::getRectSubPix(rotated, box_size, box.center, cropped);

  // cv::imshow("Original", img);
  // cv::imshow("Rotated", rotated);
  // cv::imshow("Cropped", cropped);
  // cv::waitKey(0);
  cv::bitwise_not(cropped, cropped);
  cv::imwrite("./img_pre/heb_rot.jpg", cropped);
}

int main ( int argc, char** argv )
{
  // compute_skew( "./pics/heb.jpg" );
  // deskew( "./img_pre/heb.jpg", -11.3953 );
  // deskew( "./img_pre/heb.jpg", 11.3953 );
  // deskew( "./img_pre/heb.jpg", 292.0 ); /*this is working???*/
  deskew( "./pics/heb.jpg", 289.95 ); /*this is working???*/
  // deskew( "./img_pre/heb.jpg", 248.6047 ); /*this is working???*/
  // deskew( "./img_pre/heb.jpg", 348.6 );

  return 0;
}
