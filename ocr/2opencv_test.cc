
#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char** argv )
{
  cv::Mat img = cv::imread( "./pics/tj2.jpg", 0);

  cv::bitwise_not(img, img);

  std::vector<cv::Point> points;
  cv::Mat_<uchar>::iterator it = img.begin<uchar>();
  cv::Mat_<uchar>::iterator end = img.end<uchar>();
  for (; it != end; ++it){
    if (*it) {
      points.push_back(it.pos());
    }
  }

  cv::RotatedRect box = cv::minAreaRect(cv::Mat(points));

  cv::Mat rot_mat = cv::getRotationMatrix2D(box.center, 0, 1);

  cv::Mat rotated;
  cv::warpAffine(img, rotated, rot_mat, img.size(), cv::INTER_CUBIC);

  cv::Size box_size = box.size;

  printf("width: %d, height: %d\n", box_size.width, box_size.height );

  if (box.angle < -45.)
    std::swap(box_size.width, box_size.height);

  cv::Mat cropped;
  cv::getRectSubPix ( rotated, box_size, box.center, cropped );

  // cv::imshow("Original", img);
  // cv::imshow("Rotated", rotated);
  cv::imshow("Cropped", cropped);

  cv::waitKey(0);

  return 0;
}
