
// // g++ $(pkg-config --cflags --libs opencv) longest_closed.cc -o longest_closed && ./longest_closed
// // g++ -g $(pkg-config --cflags --libs opencv) longest_closed.cc -o longest_closed

// // Longest@_closed > angle_clusters

// // longest_closed > findContours(contours) > loop contours (approxPolyDP(contoursDraw)) >
// // push to contours_long where arc_len > 5000
// // var labels =  angle_clusters > cluster to contours_l0, contours_l1 - here we get hopefully 2 angles sets with approximate 90 degree alignment
// // TODO - check the angle_clusters logic
// // coord_clusters - TODO - make dynamic for vertical/horizontal - see what happens with 45 degree angles...
// // get_closest_diagonal for (temp) left/right or future up/down lines... TODO check what happens with rotated - 45 degree lines
// // WORK, work worrk ... and then go on, get intersection points, affine transform rotate, crop etc...
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

// http://stackoverflow.com/questions/6555629/algorithm-to-detect-longest_closed-of-paper-sheet-in-photo

bool file_exists ( const std::string& name ) {

  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}
// c/c++ dummy declaration
int get_angle_approx90_count ( std::vector<cv::Point> approx, Mat drawing );
Mat angle_clusters( std::vector < std::vector<cv::Point> > contours, Mat_<float> &angles, Mat_<double> &centers );
Mat coord_clusters( Size size, std::vector < std::vector<cv::Point> > contours, Mat_<float> angles, double angle_center );
// not in use
Point2f get_mass_center(Point a, Point b);
void get_closest_diagonal ( Rect rect,  Mat_<float> angles, std::vector<cv::Point> points, Mat &pic );
static float angle_2points ( cv::Point p1, cv::Point p2 );

// // start here
void longest_closed()
{
  // Mat mat = imread( "./pics/heb.jpg");
  // Mat mat = imread( "./pics/heb2.jpg");
  // Mat mat = imread( "./pics/heb_new.jpg");
  // Mat mat = imread( "./pics/tj.jpg");
   // Mat mat = imread( "./pics/tj2.jpg");
   // Mat mat = imread( "./pics/tj22.jpg");
   // Mat mat = imread( "./pics/pers.jpg");
  // Mat mat = imread( "./pics/1.jpg"); /*bug with line clusters - hot short dotted lines*/
  Mat mat = imread( "./pics/2.jpg");
   // Mat mat = imread( "./pics/3.jpg");
   // Mat mat = imread( "./pics/4.jpg");
   // Mat mat = imread( "./pics/5.jpg");
  // Mat mat = imread( "./pics/6.jpg"); /*skewed horizontal - detects 2 points - rest should complete*/
  // Mat mat = imread( "./pics/7.jpg"); /*horizontal - dotted line - obvious imperfection with dotted line clustering algorithm   */
  // Mat mat = imread( "./pics/8.jpg"); /*almost good whole receipt - finds 3 out of 4 points - one of the angle shapes is omitted - think of algorithm variants - closed line/shape could be of several parts...*/
   // Mat mat = imread( "./pics/9.jpg"); /* flash */
   // Mat mat = imread( "./pics/10.jpg");
  // Mat mat = imread( "./pics/11.jpg"); /*example of longest shape detecting ~90 degree in the middle of a line (broken, tared paper?)*/
  // Mat mat = imread( "./pics/12.jpg"); /*skewed in middle of receipt \/\/ - no 90 degree, lines dotted lines algorithm not relevant for this case*/
  // Mat mat = imread( "./pics/13.jpg"); /*closed - worked well for 2 corners - rest are at the end of stage - TODO - algo here...*/
  // Mat mat = imread( "./pics/14.jpg"); /*same as above*/
  // Mat mat = imread( "./pics/15.jpg"); /*should have detected closed - ?? maybe not completely closed.. */
  // Mat mat = imread( "./pics/16.jpg"); /*2 points - rest at the end of stage...*/
  // Mat mat = imread( "./pics/17.jpg");/*same - 2 points....*/
  // Mat mat = imread( "./pics/18.jpg"); /*disaster with lines algorithm!!!*/

   // cleanup some images...
   remove("./img_pre/long4.jpg");
   remove("./img_pre/long5.jpg");
   remove("./img_pre/long6.jpg");

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
   std::vector<double> len_contours_contoursDraw2;
   std::vector< std::vector<cv::Point> > contours_f1;
   std::vector< std::vector<cv::Point> > contours_long;
   std::vector<std::vector<cv::Point> > contoursDraw(contours.size());
   std::vector<std::vector<cv::Point> > contoursDraw2;
   Mat poly = Mat::zeros( mat.size(), CV_8UC3 );

   double min_line_length = max(mat.size().width, mat.size().height)/10.0;
   int min_closed_line_len = (mat.size().width + mat.size().height);

   // filters out lines shorter than 200 px, straightens lines with approxPoly to contoursDraw(2), pushes to contours_long if > 5000 px..
   for (int i=0; i < (int)contours.size(); i++){

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
       if(len>min_closed_line_len) contours_long.push_back(contoursDraw[i]);
     }
   }

   Mat drawing = Mat::zeros( mat.size(), CV_8UC3 );
   Mat clong = Mat::zeros( mat.size(), CV_8UC3 );
   cv::drawContours(drawing, contours_f1, -1, cv::Scalar(0,255,0),1);

   int _angle90_count=0;
   // count the ~90 degree angles...
   for ( int i=0; i<(int)contours_long.size(); ++i ) {
     _angle90_count += get_angle_approx90_count( contours_long[i], clong );
   }

   // std::cout << " \t\t ~~~ ``` _angle90_count:" << _angle90_count << std::endl;
   // OK, this is the dotted line connection and expansion algorithm
   if ( _angle90_count<2 ) {

     Mat_<float> angles, angles0, angles1;
     Mat_<double> angle_centers;
     Mat_<int> labels = angle_clusters(contoursDraw2, angles, angle_centers); /*DONE - go on from here - add centers as ref param*/
     std::cout << "angles ref: " << angles << ", angle centers: " << angle_centers  << std::endl;

     std::vector< std::vector<cv::Point> > contours_l0;
     std::vector< std::vector<cv::Point> > contours_l1;
     double len_sum0=0.0, len_sum1=0.0;

     for(int j=0; j<labels.rows; ++j){

       if(labels(j,0)==0){
         contours_l0.push_back(contoursDraw2[j]);
         angles0.push_back(angles(j,0));
         len_sum0+=len_contours_contoursDraw2[j];
       }else if(labels(j,0)==1){
         contours_l1.push_back(contoursDraw2[j]);
         angles1.push_back(angles(j,0));
         len_sum1+=len_contours_contoursDraw2[j];
       }
     } /*separate / divide into 2 groups with approximate 90 degree alignment */

     std::cout << "angles0: " << angles0 << ',' << "angles1: " << angles1 << "c0 and c1 sizes: " << contours_l0.size() << ',' << contours_l1.size() << std::endl;

     if( contours_l0.size()>1 && len_sum0>min_line_length*5 )
       coord_clusters( mat.size(), contours_l0, angles0, angle_centers(0,0)); /*DONE then pass center[0] or centers[1] here...*/

     if( contours_l1.size()>1 && len_sum1>min_line_length*5 )
       coord_clusters( mat.size(), contours_l1, angles1, angle_centers(1,0)); /*DONE then pass center[0] or centers[1] here...*/
   }

   cv::drawContours(poly, contoursDraw2, -1, cv::Scalar(0,255,0),1);
   // cv::drawContours(poly, contours_l0, -1, cv::Scalar(0,255,0),1);
   cv::drawContours(clong, contours_long, -1, cv::Scalar(0,255,0),1);

   cv::imwrite( "./img_pre/long2.jpg", drawing);
   cv::imwrite( "./img_pre/long3.jpg", poly);
   cv::imwrite( "./img_pre/long4.jpg", clong);
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
int get_angle_approx90_count ( std::vector<cv::Point> approx, Mat drawing ) {

  // Number of vertices of polygonal curve
  int vtc = approx.size();
  // std::cout << "vtc: " << vtc << std::endl;

  // DONE - go on and crack the cos/degree thing...
  // Get the degree (in cosines) of all corners
  // std::vector<double> cos;
  double ang, ang_deg;
  int angle90_count = 0;
  std::vector<cv::Point> circles;

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

    // std::cout << "diag is: " << diag << std::endl;

    if(diag>100){
      filter_points_if_needed(circles, approx);
      int clen = circles.size();
      // std::cout << "OK, drawing circles... " << clen << std::endl;
      for ( int j=0; j<clen; ++j ) {
        cv::circle( drawing, circles[j], 50,  cv::Scalar(50,0,255) );
      }
    }

  }

  return angle90_count;
}

// the  fit line chap here man...
void get_closest_diagonal ( Rect rect,  Mat_<float> angles, std::vector<cv::Point> points, Mat &pic ) {

  // std::cout << "avg angles: " << mean(angles) << std::endl;

  // vx,vy,x,y
  // (vx, vy, x0, y0), where (vx, vy) is a normalized vector collinear to the line and (x0, y0) is a point on the line
  Vec4f line_result;
  fitLine(points, line_result, CV_DIST_L2, 0, .01, .01 );

  float vx = line_result[0];
  float vy = line_result[1];
  float x = line_result[2];
  float y = line_result[3];

  float x0, y0, x1, y1;

  x0 = x - vx*(4*pic.cols);
  y0 = y - vy*(4*pic.rows);

  x1 = x + vx*(4*pic.cols);
  y1 = y + vy*(4*pic.rows);

  // std::cout << "vec4f: " << line_result << ',' << "points: " << points << "line points" << Point(x0,y0)  << ',' << Point(x1,y1) << ',' << pic.cols << ',' << pic.rows  <<  std::endl;

  cv::line ( pic, Point(x0, y0), Point(x1, y1), cv::Scalar(0,64,255), 2, CV_AA );
}

// DONE - make dynamic here - currently assumes those are the vertical lines...
Mat coord_clusters_munge ( Size size,
                           std::vector < std::vector<cv::Point> > contours_l0, std::vector < std::vector<cv::Point> > contours_l1,
                           Mat_<float> angles0, Mat_<float> angles1
                           ) {
  Mat l0, l1;

  // if file exists - load from it - else create from zeros existing one...
  if(file_exists("./img_pre/long5.jpg"))
    l0 = imread("./img_pre/long5.jpg");
  else
    l0 = Mat::zeros( size, CV_8UC3 );

  // the second one is optional
  if(contours_l1.size()>0){

    if(file_exists("./img_pre/long6.jpg"))
      l1 = imread("./img_pre/long6.jpg");
    else
      l1 = Mat::zeros( size, CV_8UC3 );
  }

  cv::drawContours(l0, contours_l0, -1, cv::Scalar(0,255,0),1);
  // the second one is optional
  if(contours_l1.size()>0)
    cv::drawContours(l1, contours_l1, -1, cv::Scalar(255,255,0),1);

  std::vector<cv::Point> points0, points1 /*points1 is optional*/;
  for(int i=0; i<(int)contours_l0.size();++i){
    if ( contours_l0[i][0].x==0 || contours_l0[i][1].x==0 ) continue;
    points0.push_back(contours_l0[i][0]);
    points0.push_back(contours_l0[i][1]);
  }

  // the second one is optional
  if(contours_l1.size()>0){
    for(int i=0; i<(int)contours_l1.size();++i){
      if ( contours_l1[i][0].x==0 || contours_l1[i][1].x==0  ) continue;
      points1.push_back(contours_l1[i][0]);
      points1.push_back(contours_l1[i][1]);
    }
  }

  // std::cout << "points0:" << points0  << std::endl;  std::cout << "points1:" << points1  << std::endl;

  Rect r0 = cv::boundingRect(points0);

  if(points0.size()>0){
    get_closest_diagonal(r0, angles0, points0, l0);
    rectangle ( l0,r0,cv::Scalar(0,255,0) );
  }
  cv::imwrite( "./img_pre/long5.jpg", l0);

  // the second one is optional
  if ( contours_l1.size()>0 ) {
    Rect r1 = cv::boundingRect(points1);
    if(points1.size()>0){
      get_closest_diagonal(r1, angles1, points1, l1);
      rectangle ( l1,r1,cv::Scalar(0,255,0) );
    }
    cv::imwrite( "./img_pre/long6.jpg", l1);
  }

  return l0; /*dummy*/
}

double get_max_deviation(Size size, double angle_center, bool is_vert){

  double max_deviation=0.0, len_side;
  //vert - take x, hor - take y
  len_side = is_vert ? size.height : size.width;
}

Mat coord_clusters ( Size size, std::vector < std::vector<cv::Point> > contours, Mat_<float> angles, double angle_center ){

  // TODO - make get_max_deviation(size, angle_center, is_vert)
  
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

  std::cout << "\n\n ~~~~ coord clusters ~~~~ \n\n labels: is vert: "<< is_vert << '\n' << llabels << "centers" << centers << "points" << points << std::endl;

  std::vector < std::vector<cv::Point> > contours_l0, contours_l1;

  Mat_<double> centersd = centers;
  double angle_centre_diff = abs(centersd(0,0)-centersd(1,0));

  Mat_<int> labels = llabels;  Mat_<float> angles0, angles1;
  for ( int j=0; j<labels.rows; ++j ) {
    if(labels(j,0)==0){
      contours_l0.push_back(contours[j]); angles0.push_back(angles(j,0));
    }else if(labels(j,0)==1){
      contours_l1.push_back(contours[j]); angles1.push_back(angles(j,0));
    }
  }

  return coord_clusters_munge( size, contours_l0, contours_l1, angles0, angles1 );
}

Point2f get_mass_center(Point a, Point b){

  return Point2f( (int)((a.x+b.x)/2), (int)((a.y+b.y)/2) );
}

Mat angle_clusters( std::vector < std::vector<cv::Point> > contours, Mat_<float> &angles, Mat_<double> &centers ){

  // Mat angles;
  for ( int i=0; i<(int)contours.size(); ++i ) {
    angles.push_back ( angle_2points(contours[i][0], contours[i][1]) );
  }

  int clusterCount = 2;
  Mat labels;
  int attempts = 5;
  kmeans(angles, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers );

  Mat_<int> labels2 = labels;

  double angle_centre_diff = abs(centers(0,0)-centers(1,0));

  std::cout <<  "angle_centre_diff: " << angle_centre_diff  << std::endl;
  if ( angle_centre_diff > 170.0 || angle_centre_diff<10.0 ){
    for ( int j=0; j<labels.rows; ++ j ) {
      labels2(j,0) = 0;
    }
  }

  std::cout << "labels: " << labels2 << std::endl;

  return labels2;
}

// DONE go on from doing another simple loop, just to calc the angles and then try the kmean clustering... yep! may the force be with you
int main ( int argc, char** argv )
{
  // std::cout << "argc: " << argc << ',' << "argv (pointer): " << argv << std::endl;
  longest_closed();
  return 0;
}
