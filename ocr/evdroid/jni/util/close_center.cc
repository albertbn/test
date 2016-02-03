
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include "static_fields.hpp"
#include "close_center.hpp"
#include "point.hpp"
// #include "rect_poly.hpp"

using namespace cv;
using namespace std;

// custom sort function for points closest to mass center
// credits: http://answers.opencv.org/question/14188/calc-eucliadian-distance-between-two-single-point/ - check performance vs: sqrt(a.x*a.x + a.y*a.y + a.z*a.z), yep!
struct less_custom_sort_points {

    inline bool operator() (const Point& struct1, const Point& struct2)
    {
        return ( norm(center-struct1) < norm(center-struct2));
    }
};

void sort_points_closest_2center8 ( std::vector<cv::Point>& points4x ) {

  std::vector<cv::Point> points80, points81;
  std::vector<cv::Point2f> points4f;
  for ( int i=0; i<(int)points4x.size(); ++i ) {
    points4f.push_back(Point2f(points4x[i].x, points4x[i].y));
  }

  int clusterCount = 2;
  int attempts = 1;
  Mat llabels, centers;
  kmeans(points4f, clusterCount, llabels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 100, 0.0001), attempts, KMEANS_PP_CENTERS, centers );

  std::vector<int> labels = llabels;
  for ( int i=0; i<(int)labels.size(); ++i ) {
    if(labels[i])
      points81.push_back(points4x[i]);
    else
      points80.push_back(points4x[i]);
  }

  std::sort(points81.begin(), points81.end(), less_custom_sort_points());
  std::sort(points80.begin(), points80.end(), less_custom_sort_points());

  // add the first essential ones
  points4x.clear();
  points4x.push_back(points80[0]);
  points4x.push_back(points81[0]);

  // the  fill in the rest - they'll be later used for the border farthest algorithm
  for ( int i=1;   i<(int)points81.size(); ++i ) points4x.push_back(points81[i]);
  for ( int i=1;   i<(int)points80.size(); ++i ) points4x.push_back(points80[i]);
}

void sort_points_closest_2center (  std::vector<cv::Point>& points4 ) {

  // Get mass center for whole Mat/Stage
  Point center_mat(0,0);
  center_mat += Point(0,0); center_mat += Point(size_mat.width,0); center_mat += Point(size_mat.width, size_mat.height); center_mat += Point(0,size_mat.height);
  center_mat *= ( 1. / 4 );
  center = center_mat; /*global*/

  std::vector<cv::Point2f> points4f;
  for ( int i=0; i<(int)points4.size(); ++i ) {
    points4f.push_back(Point2f(points4[i].x, points4[i].y));
  }

  int clusterCount = 2;
  int attempts = 1;
  Mat llabels, centers;
  // std::vector<int> llabels, centers;
  TermCriteria tc;
  cout << "sort_points_closest_2center :: points4f " << points4f << endl;
  // return; /*TEMP*/
  kmeans(points4f, clusterCount, llabels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 100, 0.0001), attempts, KMEANS_PP_CENTERS, centers );
  // kmeans(points4f, clusterCount, llabels, tc, attempts, KMEANS_PP_CENTERS, centers );

  std::vector<int> labels = llabels;
  std::vector<cv::Point> points40, points41;
  for ( int i=0; i<(int)labels.size(); ++i ) {
    if(labels[i])
      points41.push_back(points4[i]);
    else
      points40.push_back(points4[i]);
  }

  sort_points_closest_2center8(points41/*ref*/);
  sort_points_closest_2center8(points40/*ref*/);

  points4.clear(); /*clear and re-push closest points...*/

  int to = 2; points40.size()<2 && (to=3);
  for ( int i=0; i<(int)points41.size() && i<to; ++i ){
    points4.push_back(points41[i]);
  }

  to>2 && (to=1);
  for ( int i=0; i<(int)points40.size() && i<to; ++i ){
    points4.push_back(points40[i]);
  }

  corners_magick_do(size_mat, points4 /*ref*/);

  // additional logic to choose further point
  for ( int i=0; i<(int)points4.size(); ++i ) {
    additional_logic_closest2center ( points4, points40, points41, i );
  }
}

void additional_logic_closest2center ( std::vector<cv::Point>& points4, std::vector<cv::Point>& points40, std::vector<cv::Point>& points41, int tl_br_index ) {

  std::vector<cv::Point> points_where_2search;
  Point ppoint;
  bool point_found = false;
  int point_index;

  // this folk here makes the 4 points more precise,
  // especially implements the logic that if sitting on one of the stage borders - the farthest should be used

  ppoint = points4[tl_br_index]; point_found = false; point_index = -1;
  point_found = std::find(points40.begin(), points40.end(), ppoint) != points40.end();
  if ( point_found ) {
    points_where_2search = points40;
  }
  else {
    point_found = std::find(points41.begin(), points41.end(), ppoint) != points41.end();
    if(point_found) points_where_2search = points41;

  }
  if ( !(point_found && points_where_2search.size()>2 /*!*/) ) return;

  for ( int i=0; i<(int)points_where_2search.size(); ++i ) {

    if ( point_index<0 && ppoint==points_where_2search[i] ) {
      point_index = i;
    }
    else {
      // tl, tr
      if ( tl_br_index<2 ) {
        if ( points_where_2search[i].x==ppoint.x && points_where_2search[i].y<ppoint.y ) ppoint = points4[tl_br_index] = points_where_2search[i];
      }
      //br, bl
      else {
        if ( points_where_2search[i].x==ppoint.x && points_where_2search[i].y>ppoint.y ) ppoint = points4[tl_br_index] = points_where_2search[i];
      }
    }
  }
}
