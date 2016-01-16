
#include <opencv2/opencv.hpp>
#include "static_fields.hpp"
#include "common.hpp"

using namespace cv;
using namespace std;

void filter_points_if_needed (   std::vector<cv::Point> &circles, std::vector<cv::Point> approx ) {

  cv::RotatedRect rect_minAreaRect = minAreaRect(approx);
  double area1 = contourArea(approx), area_minAreaRect;
  area_minAreaRect = rect_minAreaRect.size.width * rect_minAreaRect.size.height;
  // TODO - type - change to is_douple...
  bool is_bouble_hollow_shape = (area_minAreaRect/area1)>3.0;

  if ( !is_bouble_hollow_shape ) return;

  // if you got to here, it's a hollow rectangle - like ===\\ and should be ---\
  // that is, remove double close ~90 degree angles...
  std::vector<cv::Point2f> circle_points; /*need to convert to Point2f for kmeans */
  for ( int i=0; i<(int)circles.size(); ++i ) {
    circle_points.push_back(Point2f(circles[i].x, circles[i].y));
  }

  int clusterCount = ceil((float)circles.size()/2.0);
  int attempts = 1;
  Mat llabels, centers;
  std::vector<int> labels_dummy;
  kmeans ( circle_points, clusterCount, llabels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 100, 0.0001), attempts, KMEANS_PP_CENTERS, centers );

  std::vector<cv::Point> circles_filtered;
  Mat_<int> labels = llabels;
  for ( int j=0; j<llabels.rows; ++j ) {

    if ( std::find(labels_dummy.begin(), labels_dummy.end(), labels(j,0)) != labels_dummy.end() ) continue;

    labels_dummy.push_back(labels(j,0));
    circles_filtered.push_back(circles[j]);
  }

  circles = circles_filtered;
}

bool lines4intersect_validate ( bool is_vert, Point p_from_line, Vec4i vec4i ) {

  if ( !lines4intersect_is_vert.size() ) {
    lines4intersect.push_back(vec4i); /*add line*/
    p_from_line_vector.push_back(p_from_line); /*add point from line*/
    lines4intersect_is_vert.push_back(is_vert); /*add bool is vert*/
    return true;
  }

  int min_deviation = is_vert ? size_mat.width : size_mat.height;
  // TODO hard number
  min_deviation = min_deviation/5; /*this is the minimum distance needed between lines with same direction*/
  int x_or_y, x_or_y2;
  for ( int i=0; i<(int)lines4intersect.size(); ++i ) {

    if ( lines4intersect_is_vert[i]==is_vert) { /*found it*/
      x_or_y = is_vert ? p_from_line_vector[i].x : p_from_line_vector[i].y;
      x_or_y2 = is_vert ? p_from_line.x : p_from_line.y;

      if ( abs(x_or_y-x_or_y2)<min_deviation ) {
        return false;
      }
      break;
    }
  }

  lines4intersect.push_back(vec4i); /*add line*/
  p_from_line_vector.push_back(p_from_line); /*add point from line*/
  lines4intersect_is_vert.push_back(is_vert); /*add bool is vert*/
  return true;
}

//credits: http://answers.opencv.org/question/9511/how-to-find-the-intersection-point-of-two-lines/
bool intersection ( Point2f o1, Point2f p1, Point2f o2, Point2f p2, Point2f &r ) {
    Point2f x = o2 - o1;
    Point2f d1 = p1 - o1;
    Point2f d2 = p2 - o2;

    float cross = d1.x*d2.y - d1.y*d2.x;
    if (abs(cross) < /*EPS*/1e-8)
        return false;

    double t1 = (x.x * d2.y - x.y * d2.x)/cross;
    r = o1 + d1 * t1;
    return true;
}

cv::Point computeIntersect ( cv::Vec4i a, cv::Vec4i b ) {

  Point2f r;
  int x1 = a[0], y1 = a[1], x2 = a[2], y2 = a[3], x3 = b[0], y3 = b[1], x4 = b[2], y4 = b[3];
  intersection(Point2f(x1,y1), Point2f(x2,y2), Point2f(x3,y3), Point2f(x4,y4), r);

  return Point(r.x, r.y);
}

void intersect_n_get_points ( std::vector<cv::Point>& points4  ) {

  int ver=0, hor=0;
  for ( int i=0; i<(int)lines4intersect_is_vert.size(); ++i ) {
    (lines4intersect_is_vert[i] && ++ver) || ++hor;
  } /*! yep! */
  if ( lines4intersect.size()<4 ) {
    // add 4 border lines of the pic
    if(hor<2) lines4intersect.push_back ( cv::Vec4i(0,0,size_mat.width-5) ); /* tl-tr (hor) */
    if(ver<2) lines4intersect.push_back ( cv::Vec4i(size_mat.width,0,size_mat.width,size_mat.height-5) ); /* tr-br (ver) */
    if(hor<2) lines4intersect.push_back ( cv::Vec4i(size_mat.width,size_mat.height,5,size_mat.height) ); /* br-bl (hor) */
    if(ver<2) lines4intersect.push_back ( cv::Vec4i(0,size_mat.height,0,5) ); /* bl-tl (ver) */
  }

  // credits: http://stackoverflow.com/questions/17235987/check-if-a-cvpoint-is-inside-a-cvmat
  cv::Rect rect(cv::Point(), Size(size_mat.width+1, size_mat.height+1));

  std::vector<cv::Point> corners; cv::Point pt;
  for ( int i = 0; i < (int)lines4intersect.size(); i++ ) {

    for ( int j = i+1; j < (int)lines4intersect.size(); j++ ) {
        pt = computeIntersect(lines4intersect[i], lines4intersect[j]);
        if ( pt.x >= 0 && pt.y >= 0 &&
             (pt.x || pt.y) &&
             pt!=Point(size_mat.width,0) &&
             pt!=Point(size_mat.width,size_mat.height) &&
             pt!=Point(0,size_mat.height) &&
             rect.contains(pt)
             ){
          points4.push_back(pt);
        }
      }
  }

  Mat mb;
  if ( file_exists(path_img + "/long7.jpg") )
    mb = imread ( path_img + "/long7.jpg" );
  else
    mb = Mat::zeros ( size_mat, CV_8UC3 );

  cv::Vec4i lline;
  // std::cout << "drawing X lines:" << lines4intersect.size() << std::endl;
  for ( int i=0; i<(int)lines4intersect.size(); ++i ) {
    lline = lines4intersect[i];
    line( mb, Point(lline[0], lline[1]), Point(lline[2], lline[3]), cv::Scalar(0,255,0), 1, 8 );
  }

  cv::imwrite ( path_img + "/long7.jpg", mb ) ;
}

//1==OK t==2, b==2, 0 - not rect
bool sortCorners ( std::vector<cv::Point>& corners, cv::Point center ) {

  bool ret = false;
  std::vector<cv::Point> top, bot;

  for ( int i = 0; i < (int)corners.size(); i++ ) {

    if (corners[i].y < center.y)
      top.push_back(corners[i]);
    else
      bot.push_back(corners[i]);
  }

  if ( top.size() == 2 && bot.size() == 2 ) {

    corners.clear();
    ret = true;

    cv::Point tl = top[0].x > top[1].x ? top[1] : top[0];
    cv::Point tr = top[0].x > top[1].x ? top[0] : top[1];
    cv::Point bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
    cv::Point br = bot[0].x > bot[1].x ? bot[0] : bot[1];

    corners.push_back(tl);
    corners.push_back(tr);
    corners.push_back(br);
    corners.push_back(bl);
  }

  return ret;
}

bool corners_magick_do ( Size mat_size, std::vector<cv::Point>& corners /*points4*/ ) {

  bool are4pointsfine = false;

  center = Point(0,0);
  // Get mass center
  for ( int i = 0; i < (int)corners.size(); ++i )
    center += corners[i];
  center *= ( 1. / corners.size() );

  are4pointsfine = sortCorners ( corners, center );

  if ( !are4pointsfine ) {
    // std::cout << "The corners were not sorted correctly!" << std::endl;
    // return;
  }

  Mat m;
  if ( file_exists(path_img + "/long7.jpg") )
    m = imread ( path_img + "/long7.jpg" );
  else
    m = Mat::zeros ( mat_size, CV_8UC3 );

  // Draw lines
  for ( int i = 0; i < (int)corners.size()-1; ++i ) {
    cv::line(m, corners[i], corners[i+1], CV_RGB(0,255,0));
  }
  cv::line ( m, corners[corners.size()-1], corners[0], CV_RGB(0,255,0) );

  cv::circle ( m, center, 50, cv::Scalar(50,0,255) );

  cv::imwrite( path_img + "/long7.jpg", m);

  return are4pointsfine;
}
