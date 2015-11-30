
// g++ $(pkg-config --cflags --libs opencv) longest_closed.cc -o longest_closed && ./longest_closed
// g++ -g $(pkg-config --cflags --libs opencv) longest_closed.cc -o longest_closed

// Longest@_closed > angle_clusters

// longest_closed > findContours(contours) > loop contours (approxPolyDP(contoursDraw)) >
// push to contours_long where arc_len > 5000
// var labels =  angle_clusters > cluster to contours_l0, contours_l1 - here we get hopefully 2 angles sets with approximate 90 degree alignment
// TODO - check the angle_clusters logic
// coord_clusters - TODO - make dynamic for vertical/horizontal - see what happens with 45 degree angles...
// get_closest_diagonal for (temp) left/right or future up/down lines... TODO check what happens with rotated - 45 degree lines
// WORK, work worrk ... and then go on, get intersection points, affine transform rotate, crop etc...
// // may the force be with you

#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <math.h>
#include <iostream>
#include <sys/stat.h>

using namespace cv;
using namespace std;

std::vector<cv::Vec4i> lines4intersect;
cv::Point center(0,0);
Size size_mat;

// http://stackoverflow.com/questions/6555629/algorithm-to-detect-longest_closed-of-paper-sheet-in-photo
bool file_exists ( const std::string& name ) {

  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}
// c/c++ dummy declaration
int get_angle_approx90_count ( std::vector<cv::Point> approx, Mat drawing, std::vector<cv::Point>& points4 );
Mat angle_clusters( std::vector < std::vector<cv::Point> > contours, Mat_<float> &angles, Mat_<double> &centers );
Mat coord_clusters( Size size, std::vector < std::vector<cv::Point> > contours, Mat_<float> angles, double angle_center, std::vector<double> len_contours );
// not in use
Point2f get_mass_center(Point a, Point b);
void get_closest_diagonal ( Rect rect,  Mat_<float> angles, std::vector<cv::Point> points, Mat &pic );
static float angle_2points ( cv::Point p1, cv::Point p2 );
Mat coord_clusters_munge ( Size size,
                           std::vector < std::vector<cv::Point> > contours_l0, std::vector < std::vector<cv::Point> > contours_l1,
                           Mat_<float> angles0, Mat_<float> angles1
                           );
static void deal_with_geometry_when_not_enough_90d_angles(
                                                          Size mat_size,
                                                          std::vector<std::vector<cv::Point> > contoursDraw2,
                                                          std::vector<double> len_contours_contoursDraw2,
                                                          double min_line_length);
void reduce_noise_short_lines ( std::vector < std::vector<cv::Point> > &contours, Mat_<float> &angles, std::vector<double> len_contours);

void final_magic_crop_rotate ( Mat mat,  std::vector<cv::Point>& points4 );

//credits: http://answers.opencv.org/question/9511/how-to-find-the-intersection-point-of-two-lines/
bool intersection(Point2f o1, Point2f p1, Point2f o2, Point2f p2, Point2f &r){

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

// not everyday is Easter :) - check the intersect thing...
cv::Point computeIntersect ( cv::Vec4i a, cv::Vec4i b ) {

  Point2f r;
  int x1 = a[0], y1 = a[1], x2 = a[2], y2 = a[3], x3 = b[0], y3 = b[1], x4 = b[2], y4 = b[3];
  intersection(Point2f(x1,y1), Point2f(x2,y2), Point2f(x3,y3), Point2f(x4,y4), r);

  // std::cout << "intersection: " << r << std::endl;
  return Point(r.x, r.y);
}

void intersect_n_get_points ( std::vector<cv::Point>& points4  ) {

  if ( lines4intersect.size()<4 ) {
    // add 4 border lines of the pic
    lines4intersect.push_back ( cv::Vec4i(0,0,size_mat.width-1,0) ); /* tl-tr */
    lines4intersect.push_back ( cv::Vec4i(size_mat.width,0,size_mat.width,size_mat.height-1) ); /* tr-br */
    lines4intersect.push_back ( cv::Vec4i(size_mat.width,size_mat.height,1,size_mat.height) ); /* br-bl */
    lines4intersect.push_back ( cv::Vec4i(0,size_mat.height,0,1) ); /* bl-tl */
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
  if ( file_exists("./img_pre/long7.jpg") )
    mb = imread ( "./img_pre/long7.jpg" );
  else
    mb = Mat::zeros ( size_mat, CV_8UC3 );

  cv::Vec4i lline;
  for ( int i=0; i<(int)lines4intersect.size(); ++i ) {
    lline = lines4intersect[i];
    line( mb, Point(lline[0], lline[1]), Point(lline[2], lline[3]), cv::Scalar(0,255,0), 1, 8 );
  }

  cv::imwrite ( "./img_pre/long7.jpg", mb ) ;
}

// shall we yep? - sort corners
//1==OK t==2, b==2, 0 - not rect
bool sortCorners(std::vector<cv::Point>& corners, cv::Point center) {

  bool ret = false;
  std::vector<cv::Point> top, bot;

  for (int i = 0; i < (int)corners.size(); i++)
    {
      if (corners[i].y < center.y)
        top.push_back(corners[i]);
      else
        bot.push_back(corners[i]);
    }
  //std::cout << "top, bot, corn, center: " << top << ',' << bot << ',' << corners << ',' << center << std::endl;

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

bool corners_magick_do( Size mat_size, std::vector<cv::Point>& corners /*points4*/ ){

  bool are4pointsfine = false;

  // Get mass center
  for ( int i = 0; i < (int)corners.size(); ++i )
    center += corners[i];
  center *= ( 1. / corners.size() );

  //std::cout << "corners, center: " << corners << ',' << center << std::endl;

 are4pointsfine = sortCorners ( corners, center );

  if ( !are4pointsfine ) {
    //std::cout << "The corners were not sorted correctly!" << corners << std::endl;
    // return;
  }

  Mat m;
  if(file_exists("./img_pre/long7.jpg"))
      m = imread("./img_pre/long7.jpg");
    else
      m = Mat::zeros( mat_size, CV_8UC3 );

  // Draw lines
  for (int i = 0; i < (int)corners.size()-1; ++i){
    cv::line(m, corners[i], corners[i+1], CV_RGB(0,255,0));
  }
  cv::line(m, corners[corners.size()-1], corners[0], CV_RGB(0,255,0));


  cv::circle ( m, center, 50, cv::Scalar(50,0,255) );

  cv::imwrite( "./img_pre/long7.jpg", m);

  return are4pointsfine;
}

double MIN_LINE_LENGTH_CONSIDERED_SIDE;
// start here
void longest_closed()
{
  // Mat mat = imread( "./pics/11.jpg"); /* :) TODO - yep! example of longest shape detecting ~90 degree in the middle of a line (broken, tared paper?)*/
  // Mat mat = imread( "./pics/2.jpg"); /*TODO - for single line, skip - dotted single side or receipt*/

  Mat mat = imread( "./pics/4.jpg"); /*TODO - implement the smallest/closest to center points when we have 3 lines (corners are more than 4) */
  // Mat mat = imread( "./pics/5.jpg"); /*TODO - same closest 2 center*/
  // Mat mat = imread( "./pics/6.jpg"); /*TODO - same c2c yep!*/

  // Mat mat = imread( "./pics/8.jpg"); /*TODO - same c2c - yep!*/
  // Mat mat = imread( "./pics/13.jpg"); /*TODO -  c2c yep! closed - worked well for 2 corners - rest are at the end of stage*/
  // Mat mat = imread( "./pics/14.jpg"); /*TODO - c2c - yep! same as above*/
  // Mat mat = imread( "./pics/15.jpg"); /*TODO - c2c - yep! should have detected closed - ?? maybe not completely closed.. */
  // Mat mat = imread( "./pics/16.jpg"); /*TODO - c2c - yep! 2 points - rest at the end of stage...*/
  // Mat mat = imread( "./pics/17.jpg");/*TODO - c2c - yep! same - 2 points....*/
  // Mat mat = imread( "./pics/9.jpg"); /*TODO - c2c -yep! flash - yep! with a little work with fitLine angle and double line...  */
  // Mat mat = imread( "./pics/10.jpg"); /*TODO - c2c - yep!*/

  // Mat mat = imread( "./pics/12.jpg"); /* TODO - lines skewed... yep! skewed in middle of receipt \/\/ - no 90 degree, lines dotted lines algorithm not relevant for this case  */

  //---------------
  // Mat mat = imread( "./pics/18.jpg"); /*TODO - calc quad from perspective... - learn, yep!*/

  // Mat mat = imread( "./pics/heb.jpg"); /*yep!*/
  // Mat mat = imread( "./pics/heb2.jpg"); /*yep!*/
  // Mat mat = imread( "./pics/heb_new.jpg"); /*yep indeed - check also if closed when 4 * 90 deg found - also if form is a satisfying rectangular?*/

  // Mat mat = imread( "./pics/tj.jpg");
  // Mat mat = imread( "./pics/tj2.jpg");
  // Mat mat = imread( "./pics/tj22.jpg");
  // Mat mat = imread( "./pics/1.jpg"); /*bug solved with line clusters - hot short dotted lines GOON from here - 1 small line missing... check*/
  // Mat mat = imread( "./pics/7.jpg"); /*horizontal - dotted line - obvious imperfection with dotted line clustering algorithm TODO - maybe clear noise by avg longest...   */
  // Mat mat = imread( "./pics/3.jpg"); /*yep*/
  // Yep. but for me for now it's perfect ;)

  // Mat mat = imread( "./pics/pers.jpg"); /*kidding? :)*/

  // cleanup some images...
  remove("./img_pre/long4.jpg");
  remove("./img_pre/long5.jpg");
  remove("./img_pre/long6.jpg");
  remove("./img_pre/long7.jpg");
  remove("./img_pre/long8.jpg");
  remove("./img_pre/long44.jpg");
  remove("./img_pre/long444.jpg");

  size_mat = mat.size();

  cv::cvtColor(mat, mat, CV_BGR2GRAY);

  // std::cout << mat << std::endl;

  /// Apply Histogram Equalization - not clear - sometimes is good, sometimes not???
  // equalizeHist ( mat, mat );

  // cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
  // clahe->setClipLimit(1);
  // clahe->setTilesGridSize( Size(150,150) );
  // clahe->apply(mat,mat);

  // cv::GaussianBlur(mat, mat, cv::Size(3,3), 0);
  // blur(mat, mat, Size(20,20));
  cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Point(19,19));
  cv::Mat dilated;
  cv::dilate(mat, dilated, kernel);

  blur(dilated, dilated, Size(10,10));

  cv::imwrite( "./img_pre/long0.jpg", dilated );

  cv::Mat edges;
  cv::Canny(dilated, edges, 40, 1);
  blur(edges, edges, Size(10,10));

  cv::imwrite( "./img_pre/long1.jpg", edges);

  std::vector< std::vector<cv::Point> > contours;
  // cv::findContours(edges, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
  cv::findContours(edges, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS);

  // std::cout << "contours count: " <<  contours.size()  << std::endl;

  // DONE? - go on from checking if the >10000 is a single len
  double len;
  std::vector<double> len_contours_contoursDraw2, len_contours_closed;
  std::vector< std::vector<cv::Point> > contours_f1;
  std::vector< std::vector<cv::Point> > contours_long, contours_medium;
  std::vector<std::vector<cv::Point> > contoursDraw(contours.size());
  std::vector<std::vector<cv::Point> > contoursDraw2;
  Mat poly = Mat::zeros ( mat.size(), CV_8UC3 ) ;

  double min_line_length = MIN_LINE_LENGTH_CONSIDERED_SIDE = max(mat.size().width, mat.size().height)/13.0; /*TODO - here - check this chap*/
  MIN_LINE_LENGTH_CONSIDERED_SIDE*=4.5; /*this is a TODO for sure - should implement some other algo for \/  / clustered in vert - pics/18.jpg*/
  int min_closed_line_len = (mat.size().width + mat.size().height);

  // fills contoursDraw2 :: filters out lines shorter than 200 px, straightens lines with approxPoly to contoursDraw(2), pushes to contours_long if > 5000 px..
  for ( int i=0; i < (int)contours.size(); i++ ) {

    len = cv::arcLength(contours[i], true);
    if(len < min_line_length) {
      // std::cout << "length smaller than min_line_length...: " << len << ',' << min_line_length << std::endl;
      continue;
    }
    len_contours_contoursDraw2.push_back(len);

    cv::approxPolyDP(Mat(contours[i]), contoursDraw[i], 40, true);
    contoursDraw2.push_back(contoursDraw[i]);

    if(len>0){
      // std::cout << "closed line len...: " << len << std::endl;
      contours_f1.push_back(contours[i]);
      if(len>min_closed_line_len) {
        contours_long.push_back(contoursDraw[i]);
        len_contours_closed.push_back(len);
      }
      else if(len>(min_closed_line_len/5)){
        contours_medium.push_back(contoursDraw[i]);
        len_contours_closed.push_back(len);
      }
    }
  }

  Mat drawing = Mat::zeros( mat.size(), CV_8UC3 );
  Mat clong = Mat::zeros( mat.size(), CV_8UC3 );
  cv::drawContours(drawing, contours_f1, -1, cv::Scalar(0,255,0),1);

  int _angle90_count=0; std::vector<cv::Point> points4;
  // count the ~90 degree angles...
  for ( int i=0; i<(int)contours_long.size(); ++i ) {
    _angle90_count += get_angle_approx90_count ( contours_long[i], clong, points4/*ref*/ );
  }

  // DONE, yep! - somewhere here start and implement the persp.cc - good luck - calc center, order points, etc...
  //std::cout << " \t\t ~~~ ``` _angle90_count:" << _angle90_count << std::endl;
  // OK, this is the dotted line connection and expansion algorithm
  if ( _angle90_count!=4 || !corners_magick_do(mat.size(), points4 /*a 4 point chap - validate this folk*/) ) {

    // TODO - add logic here for using just longest and parts... for cases where there is longest and at least 1 90 deg angle...
    if ( contours_long.size() || contours_medium.size() ){
      for(int i=0; i<(int)contours_long.size(); ++i) { contours_medium.push_back(contours_long[i]); }

      deal_with_geometry_when_not_enough_90d_angles( mat.size(), contours_medium, len_contours_closed, min_line_length);
    }
    else {
      deal_with_geometry_when_not_enough_90d_angles( mat.size(), contoursDraw2, len_contours_contoursDraw2, min_line_length);
    }

    if ( lines4intersect.size()<4 ) {
      points4.clear();
      intersect_n_get_points ( points4 /*ref*/ );
      corners_magick_do(mat.size(), points4 /*a 4 point chap - validate this folk*/);
    }
    final_magic_crop_rotate (  mat, points4 /*ref*/ );

    std::cout << "lines4intersect size: " << lines4intersect.size() << ",\n points4: " << points4 << std::endl;
  }
  else {
    final_magic_crop_rotate (  mat, points4 /*ref*/ );
  }

  cv::drawContours(poly, contoursDraw2, -1, cv::Scalar(0,255,0),1);
  cv::drawContours(clong, contours_long, -1, cv::Scalar(0,255,0),1);

  cv::imwrite( "./img_pre/long2.jpg", drawing);
  cv::imwrite( "./img_pre/long3.jpg", poly);
  cv::imwrite( "./img_pre/long4.jpg", clong);
}

// custom sort function for points closest to mass center
// credits: http://answers.opencv.org/question/14188/calc-eucliadian-distance-between-two-single-point/ - check performance vs: sqrt(a.x*a.x + a.y*a.y + a.z*a.z), yep!
struct less_custom_sort_points {

    inline bool operator() (const Point& struct1, const Point& struct2)
    {
        return ( norm(center-struct1) < norm(center-struct2));
    }
};

void sort_points_closest_2center (  std::vector<cv::Point>& points4 ) {

  // Get mass center for whole Mat/Stage
  Point center_mat(0,0);
  center_mat += Point(0,0); center_mat += Point(size_mat.width,0); center_mat += Point(size_mat.width, size_mat.height); center_mat += Point(0,size_mat.height);
  center_mat *= ( 1. / 4 );
  center = center_mat;

  std::vector<cv::Point2f> points4f; /*need to convert to Point2f for kmeans */
  for ( int i=0; i<(int)points4.size(); ++i ) {
    points4f.push_back(Point2f(points4[i].x, points4[i].y));
  }

  int clusterCount = 2;
  int attempts = 1;
  Mat llabels, centers;
  kmeans(points4f, clusterCount, llabels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 100, 0.0001), attempts, KMEANS_PP_CENTERS, centers );
  // std::cout << "\n\n clustered points: \n\n labels: " << llabels << "\ncenters: " << centers << "\npoints4f: " << points4f << std::endl;

  std::vector<int> labels = llabels;
  std::vector<cv::Point> points40, points41;
  for ( int i=0; i<(int)labels.size(); ++i ) {
    if(labels[i])
      points41.push_back(points4[i]);
    else
      points40.push_back(points4[i]);
  }

  if ( points41.size()>2 ) {
    std::sort(points41.begin(), points41.end(), less_custom_sort_points());
  }

  if ( points40.size()>2 ) {
    std::sort(points40.begin(), points40.end(), less_custom_sort_points());
  }

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

  std::cout << "p40, p41, p4: " << points40 << points41 << points4 << std::endl;
}

void final_magic_crop_rotate ( Mat mat,  std::vector<cv::Point>& points4 ) {

  if ( points4.size()>4 ){
    sort_points_closest_2center(points4);
    Point pfirst = points4[0]; points4.erase(points4.begin());
    points4.push_back(pfirst);
  }

  Mat mb;
  if ( file_exists("./img_pre/long7.jpg") )
    mb = imread("./img_pre/long7.jpg");
  else
    mb = Mat::zeros( mat.size(), CV_8UC3 );

  std::vector<cv::Point2f> points4f;
  // this here is probably closest to the size of the original invoice... well, let's try... tension :)
  cv::RotatedRect rect_minAreaRect = minAreaRect(points4);

  RNG rng(12345);
  Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
  Point2f rect_points[4]; rect_minAreaRect.points( rect_points );

  for ( int i=0; i<(int)points4.size(/*4*/); ++i ) {
    points4f.push_back(points4[i]);
    line( mb, rect_points[i], rect_points[(i+1)%4], color, 1, 8 );
    cv::circle ( mb, points4[i], 50, cv::Scalar(50,0,255) );
  }

  bool is_mat_width = size_mat.width>size_mat.height; /*is width larger*/
  int small = min(rect_minAreaRect.size.width, rect_minAreaRect.size.height);
  int large = max(rect_minAreaRect.size.width, rect_minAreaRect.size.height);
  !is_mat_width && (small=small^large) && (large=small^large) && (small=small^large); /*XOR swap*/
  cv::Mat quad = cv::Mat::zeros ( small, large, CV_8UC3 );

  std::vector<cv::Point2f> quad_pts;
  quad_pts.push_back(cv::Point2f(0, 0));
  quad_pts.push_back(cv::Point2f(quad.cols, 0));
  quad_pts.push_back(cv::Point2f(quad.cols, quad.rows));
  quad_pts.push_back(cv::Point2f(0, quad.rows));

  if ( points4f.size()==4 ) {
    cv::Mat transmtx = cv::getPerspectiveTransform ( points4f, quad_pts );
    cv::warpPerspective ( mat, quad, transmtx, quad.size() );
  }
  else{
    std::cout << "checking points4f... " << points4f << std::endl;
  }

  cv::imwrite( "./img_pre/long7.jpg", mb);
  cv::imwrite( "./img_pre/long8.jpg", quad);
}

// splits contours to dotted lines, no matter if closed or not - should get just a collection of 2 point straight vanilla lines
static void split_contours_2_dotted_lines( std::vector<std::vector<cv::Point> > &contoursDraw2, std::vector<double> &len_contours_contoursDraw2, double min_line_length ){

  std::vector<std::vector<cv::Point> > contoursDraw3;
  std::vector<cv::Point> line_tmp;
  std::vector<double> len_contours_contoursDraw3;
  double len;

  for ( int i=0; i < (int)contoursDraw2.size(); i++ ) {

    // a regular 2 point line
    if ( contoursDraw2[i].size()<3 ){

      len = cv::arcLength(contoursDraw2[i], true);
      if ( len>min_line_length ) {
        contoursDraw3.push_back(contoursDraw2[i]);
        len_contours_contoursDraw3.push_back(len);
      }
      continue;
    }
    int ssize = (int)contoursDraw2[i].size();
    // for ( int j=0; j<ssize; ++j ) {
    for ( int j=0; j<ssize-1; ++j ) { /*don't do last with first - that is, don't plot a closing line */
      line_tmp.clear();
      line_tmp.push_back( contoursDraw2[i][j] );
      line_tmp.push_back(contoursDraw2[i][j+1]);
      // (j<ssize-1) ? line_tmp.push_back( contoursDraw2[i][j+1] ) : line_tmp.push_back( contoursDraw2[i][0] ); /*connect last dot to first one???*/
      len = cv::arcLength(line_tmp, true);
      if ( len>min_line_length ) {
        contoursDraw3.push_back( line_tmp  );
        len_contours_contoursDraw3.push_back(len);
      }
    }
  }

  contoursDraw2 = contoursDraw3;
  len_contours_contoursDraw2 = len_contours_contoursDraw3;
}

static void deal_with_geometry_when_not_enough_90d_angles(
                                                          Size mat_size,
                                                          std::vector<std::vector<cv::Point> > contoursDraw2,
                                                          std::vector<double> len_contours_contoursDraw2,
                                                          double min_line_length
                                                          ){

  // std::cout << "dwgwne9a: cd2: " << contoursDraw2.size() << std::endl;

  // Mat l44, l444;
  // if(file_exists("./img_pre/long44.jpg"))
  //   l44 = imread("./img_pre/long44.jpg");
  // else
  //   l44 = Mat::zeros( mat_size, CV_8UC3 );

  // if(file_exists("./img_pre/long444.jpg"))
  //   l444 = imread("./img_pre/long444.jpg");
  // else
  //   l444 = Mat::zeros( mat_size, CV_8UC3 );

  // // before split contours
  // cv::drawContours(l44, contoursDraw2, -1, cv::Scalar(0,255,0),1);
  // cv::imwrite( "./img_pre/long44.jpg", l44);

  // shall we work? - well :) maybe - c u next time :) suck Shawn, suck
  split_contours_2_dotted_lines ( /*ref*/contoursDraw2, /*ref*/len_contours_contoursDraw2, min_line_length );
  // std::cout << "\n\ncd2 len:" << contoursDraw2.size() << "\n\ncd2 itself: " << Mat(contoursDraw2) << ", \n\nlen_contours_contoursDraw2: " << Mat(len_contours_contoursDraw2) << std::endl;

  // after split contours
  // cv::drawContours(l444, contoursDraw2, -1, cv::Scalar(0,255,0),1);
  // cv::imwrite( "./img_pre/long444.jpg", l444);

  Mat_<float> angles, angles0, angles1;
  Mat_<double> angle_centers;
  Mat_<int> labels = angle_clusters(contoursDraw2, angles, angle_centers); /*OK if a single line,*/
  // std::cout << "angles ref: " << angles << ", angle centers: " << angle_centers  << std::endl;

  std::vector< std::vector<cv::Point> > contours_l0;
  std::vector< std::vector<cv::Point> > contours_l1;
  double len_sum0=0.0, len_sum1=0.0;
  std::vector<double> len_contours0, len_contours1;

  /*separate / divide into 2 groups with approximate 90 degree alignment */
  for(int j=0; j<labels.rows; ++j){

    if(labels(j,0)==0){
      contours_l0.push_back(contoursDraw2[j]);
      angles0.push_back(angles(j,0));
      len_sum0+=len_contours_contoursDraw2[j];
      len_contours0.push_back(len_contours_contoursDraw2[j]);
    }else if(labels(j,0)==1){
      contours_l1.push_back(contoursDraw2[j]);
      angles1.push_back(angles(j,0));
      len_sum1+=len_contours_contoursDraw2[j];
      len_contours1.push_back(len_contours_contoursDraw2[j]);
    }
  }

  // std::cout << "angle_centers: " << angle_centers << "\n angles0: " << angles0 << ',' << "angles1: " << angles1 << "c0 and c1 sizes: " << contours_l0.size() << ',' << contours_l1.size() << "\nlen_sum0, len_sum1: " << len_sum0 << ',' << len_sum1 << "\nmin_line_length*5: " << min_line_length*5 << std::endl;

  //std::cout << "\n~~~ \n len_contours0, len_contours1:" << Mat(len_contours0) << ',' << Mat(len_contours1) << std::endl;

  std::vector< std::vector<cv::Point> > dumm; Mat_<float> angles_dumm; /*2 dummies used as null pointers - no time to learn c++ :) */

  if ( contours_l0.size()>1 && len_sum0>min_line_length*5 )
    coord_clusters( mat_size, contours_l0, angles0, angle_centers(0,0), len_contours0); /*DONE then pass center[0] or centers[1] here...*/
  else if ( contours_l0.size()<2 && len_sum0>min_line_length*5 ){
    coord_clusters_munge( mat_size, contours_l0, dumm, angles0, angles_dumm );
  }

  if( contours_l1.size()>1 && len_sum1>min_line_length*5 )
    coord_clusters( mat_size, contours_l1, angles1, angle_centers(1,0), len_contours1); /*DONE then pass center[0] or centers[1] here...*/
  else if( contours_l1.size()<2 && len_sum1>min_line_length*5 ){
    coord_clusters_munge( mat_size, contours_l1, dumm, angles1, angles_dumm );
  }
}

static float angle_2points ( cv::Point p1, cv::Point p2 ) {

  float ang = atan2(p1.y - p2.y, p1.x - p2.x);
  // ang = ang * 180 / CV_PI;
  ang = abs(ang * 180 / CV_PI);
  ang<90.0 && (ang=180.0-ang); /*put all folks in the top left quadrant of the 360 circle - used only for kmeans clustering*/
  // std::cout << "area: " << area1 << " ,minAreaRect area:" << area_minAreaRect  << std::endl;
  // return (ang>0.0) ? ang : 180.0 + ang;
  // return abs(ang);
  return ang;
}

static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
        double cos_value = (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);

        return acos(cos_value) * 180.0 / CV_PI;
}

// get the diagonal of the bounding rectangle...
double get_longest_side_poly( std::vector<cv::Point> approx ){

  Rect rect = boundingRect(approx);

  return sqrt(rect.width*rect.width + rect.height*rect.height );
}

void filter_points_if_needed (   std::vector<cv::Point> &circles, std::vector<cv::Point> approx ){

  cv::RotatedRect rect_minAreaRect = minAreaRect(approx);
  double area1 = contourArea(approx), area_minAreaRect;
  area_minAreaRect = rect_minAreaRect.size.width * rect_minAreaRect.size.height;
  // std::cout << "area: " << area1 << " ,minAreaRect area:" << area_minAreaRect  << std::endl;
  // DONE - go on from here... - divide the area (width * height of the minArea) and then divide by the contourArea - and see if result is (>2,3,4) larger than say 2...
  bool is_bouble_hollow_shape = (area_minAreaRect/area1)>3.0;

  if(!is_bouble_hollow_shape) return;

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
  kmeans(circle_points, clusterCount, llabels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 100, 0.0001), attempts, KMEANS_PP_CENTERS, centers );
  // std::cout << "\n\n ~~~~ filter points ~~~~ \n\n labels: " << llabels << "centers" << centers << "circle_points" << circle_points << std::endl;

  std::vector<cv::Point> circles_filtered;
  Mat_<int> labels = llabels;
  for ( int j=0; j<llabels.rows; ++j ) {

    if ( std::find(labels_dummy.begin(), labels_dummy.end(), labels(j,0)) != labels_dummy.end() )
      continue;

    labels_dummy.push_back(labels(j,0));
    circles_filtered.push_back(circles[j]);
  }

  circles = circles_filtered;
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

// the  fit line chap here man...
void get_closest_diagonal ( Rect rect,  Mat_<float> angles, std::vector<cv::Point> points, Mat &pic ) {

  std::cout << "\n§§§§§§§\navg angles: " << mean(angles)[0] << std::endl;

  // vx,vy,x,y
  // (vx, vy, x0, y0), where (vx, vy) is a normalized vector collinear to the line and (x0, y0) is a point on the line
  Vec4f line_result;
  fitLine(points, line_result, CV_DIST_L2, 0, .01, .01 );

  float vx = line_result[0];
  float vy = line_result[1];
  float x = line_result[2];
  float y = line_result[3];

  float x0, y0, x1, y1;

  x0 = x - vx*(2*pic.cols);
  y0 = y - vy*(2*pic.rows);

  x1 = x + vx*(2*pic.cols);
  y1 = y + vy*(2*pic.rows);

  std::cout << "\nvec4f: " << line_result << ',' << ",\nline points: " << Point(x0,y0)  << ',' << Point(x1,y1) << std::endl;

  // going on from intersect and gathering 4 points, yep!
  cv::line ( pic, Point(x0, y0), Point(x1, y1), cv::Scalar(0,64,255), 2, CV_AA );

  // global chap - fill it with lines
  lines4intersect.push_back ( cv::Vec4i(x0,y0,x1,y1) );
}

Mat coord_clusters_munge ( Size size,
                           std::vector < std::vector<cv::Point> > contours_l0, std::vector < std::vector<cv::Point> > contours_l1,
                           Mat_<float> angles0, Mat_<float> angles1
                           ) {
  Mat l0, l1;

  // if file exists - load from it - else create from zeros existing one...
  if(contours_l0.size()>0){
    if(file_exists("./img_pre/long5.jpg"))
      l0 = imread("./img_pre/long5.jpg");
    else
      l0 = Mat::zeros( size, CV_8UC3 );
  }
  // the second one is optional
  if(contours_l1.size()>0){

    if(file_exists("./img_pre/long6.jpg"))
      l1 = imread("./img_pre/long6.jpg");
    else
      l1 = Mat::zeros( size, CV_8UC3 );
  }

  if(contours_l0.size()>0)
    cv::drawContours(l0, contours_l0, -1, cv::Scalar(0,255,0),1);
  // the second one is optional
  if(contours_l1.size()>0)
    cv::drawContours(l1, contours_l1, -1, cv::Scalar(255,255,0),1);

  std::vector<cv::Point> points0, points1 /*points1 is optional*/;
  if(contours_l0.size()>0){
    for(int i=0; i<(int)contours_l0.size();++i){
      if ( contours_l0[i][0].x==0 || contours_l0[i][1].x==0 ) continue;
      points0.push_back(contours_l0[i][0]);
      points0.push_back(contours_l0[i][1]);
      // std::cout << "c0 len: " << arcLength(contours_l0[i], true) << std::endl;
    }
  }

  // the second one is optional
  if(contours_l1.size()>0){
    for(int i=0; i<(int)contours_l1.size();++i){
      if ( contours_l1[i][0].x==0 || contours_l1[i][1].x==0  ) continue;
      points1.push_back(contours_l1[i][0]);
      points1.push_back(contours_l1[i][1]);
      // std::cout << "c1 len: " << arcLength(contours_l1[i], true) << std::endl;
    }
  }

  // std::cout << "points0:" << points0  << std::endl;  std::cout << "points1:" << points1 << ", co, c1 sizes: " << contours_l0.size() << ',' << contours_l1.size()  << std::endl;

  if ( contours_l0.size()>0 ) {
    Rect r0 = cv::boundingRect(points0);
    if(points0.size()>0){
      get_closest_diagonal(r0, angles0, points0, l0);
      // rectangle ( l0,r0,cv::Scalar(0,255,0) );
    }
    cv::imwrite( "./img_pre/long5.jpg", l0);
  }

  // the second one is optional
  if ( contours_l1.size()>0 ) {
    Rect r1 = cv::boundingRect(points1);
    if(points1.size()>0){
      get_closest_diagonal(r1, angles1, points1, l1);
      // rectangle ( l1,r1,cv::Scalar(0,255,0) );
    }
    cv::imwrite( "./img_pre/long6.jpg", l1);
  }

  return l0; /*dummy*/
}

double get_max_deviation(Size size, double angle_center, bool is_vert){

  double max_deviation=0.0, len_side, angle_center_rad;
  len_side = is_vert ? size.height : size.width;
  angle_center = is_vert ? angle_center-90.0 : 180-angle_center;
  angle_center_rad = angle_center/180*CV_PI;
  max_deviation = abs ( tan(angle_center_rad) * len_side ) ;

  // std::cout << "angle_center: " << angle_center << ", len_side: " << len_side << ", angle_center_rad: " << angle_center_rad << ", max_deviation: " << max_deviation  <<  std::endl;
  return max_deviation;
}

// TODO - go on from here - think whether to clean noise before or after grouping... - maybe after for now?
Mat coord_clusters ( Size size, std::vector < std::vector<cv::Point> > contours, Mat_<float> angles, double angle_center, std::vector<double> len_contours ){

  bool is_vert = angle_center > 45.0 && angle_center < 135.0; /*between 45 degrees trough 90 degrees to 135 degrees is considered vertical*/
  std::vector<cv::Point2f> points;
  for(int i=0; i<(int)contours.size(); ++i){

    if ( is_vert ) {
      points.push_back( Point2f(contours[i][0].x, 0) );
    }
    else {
      points.push_back( Point2f(0, contours[i][0].y) );
    }
  }

  int clusterCount =  2;
  int attempts = 1;
  Mat llabels, centers;
  kmeans(points, clusterCount, llabels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 100, 0.0001), attempts, KMEANS_PP_CENTERS, centers );

  std::vector < std::vector<cv::Point> > contours_l0, contours_l1;

  Mat_<double> centersd = centers;
  Mat_<int> labels = llabels;  Mat_<float> angles0, angles1;
  double xy_centre_diff = is_vert ? abs(centersd(0,0)-centersd(1,0)) : abs(centersd(0,1)-centersd(1,1));

  // this here means there is just 1 group of hor or vert lines
  if( get_max_deviation(size, angle_center, is_vert) > xy_centre_diff ){
    for ( int j=0; j<labels.rows; ++ j ) {
      labels(j,0) = 0;
    }
  }

  // std::cout << "\n\n ~~~~ coord clusters ~~~~ \n\n labels: is vert: "<< is_vert << '\n' << llabels << "centers" << centers << "points" << points << '\n' << "xy_centre_diff: " << xy_centre_diff << std::endl;

  std::vector<double> len_contours0, len_contours1;

  for ( int j=0; j<labels.rows; ++j ) {
    if(labels(j,0)==0){
      contours_l0.push_back(contours[j]); angles0.push_back(angles(j,0));
      len_contours0.push_back(len_contours[j]);
      // len_contours0.push_back( arcLength(contours[j],true));
    }else if(labels(j,0)==1){
      contours_l1.push_back(contours[j]); angles1.push_back(angles(j,0));
      len_contours1.push_back(len_contours[j]);
      // len_contours1.push_back(arcLength(contours[j],true));
    }
  }

  // std::cout << "\n\n ~~~ c0, c1 sizes: " << contours_l0.size() << ',' << contours_l1.size() << std::endl;

  // go on from here - check for size > 1 maybe..., declare fn below...
  if( (int)contours_l0.size()>0) reduce_noise_short_lines( contours_l0, angles0, len_contours0);
  if( (int)contours_l1.size()>0) reduce_noise_short_lines( contours_l1, angles1, len_contours1);

  return coord_clusters_munge( size, contours_l0, contours_l1, angles0, angles1 );
}

void reduce_noise_short_lines ( std::vector < std::vector<cv::Point> > &contours, Mat_<float> &angles, std::vector<double> len_contours){

  Mat m ( len_contours );
  cv::Scalar mean, stdev;
  cv::meanStdDev(m, mean, stdev);

  std::vector < std::vector<cv::Point> > contours2;
  Mat_<float> angles2;
  std::vector<double>::iterator biggest = std::max_element(len_contours.begin(), len_contours.end());
  double d_stdev = stdev[0] / (*biggest / stdev[0]);

  //std::cout << "d_stdev: " << d_stdev << ", len_contours: " << m << ", mean: " << mean << ',' << "stdev: " << stdev << ", contours.size: " << contours.size() << ", angles.size: " << angles.size() << std::endl << std::endl;
  float len_total = 0;
  for(int i=0; i<(int)len_contours.size(); ++i){
    if(len_contours[i]>=d_stdev){
      contours2.push_back(contours[i]);
      angles2.push_back(angles(0,i));
      len_total+=len_contours[i];
    }
  }

  if ( len_total < MIN_LINE_LENGTH_CONSIDERED_SIDE )
    contours2.clear();

  contours = contours2; angles = angles2; /*TODO - go on from here */
}

Point2f get_mass_center(Point a, Point b){

  return Point2f( (int)((a.x+b.x)/2), (int)((a.y+b.y)/2) );
}

Mat angle_clusters( std::vector < std::vector<cv::Point> > contours, Mat_<float> &angles, Mat_<double> &centers ){

  int clusterCount = 2;
  Mat labels;
  Mat_<int> labels2;

  // Mat angles;
  for ( int i=0; i<(int)contours.size(); ++i ) {
    angles.push_back ( angle_2points(contours[i][0], contours[i][1]) );
  }

  // check if there was just one line - then return here
  if( (int)contours.size() < clusterCount){
    labels2.push_back(0);
    centers.push_back(angles.at<float>(0,0));
    return labels2;
  }

  int attempts = 5;
  kmeans(angles, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers );

  labels2 = labels;
  double angle_centre_diff = abs(centers(0,0)-centers(1,0));

  // std::cout <<  "angle_centre_diff: " << angle_centre_diff  << std::endl;
  if ( angle_centre_diff > 170.0 || angle_centre_diff<10.0 ){
    for ( int j=0; j<labels.rows; ++ j ) {
      labels2(j,0) = 0;
    }
  }

  // std::cout << "labels: " << labels2 << std::endl;

  return labels2;
}

// DONE go on from doing another simple loop, just to calc the angles and then try the kmean clustering... yep! may the force be with you
int main ( int argc, char** argv )
{
  // std::cout << "argc: " << argc << ',' << "argv (pointer): " << argv << std::endl;
  longest_closed();
  return 0;
}
