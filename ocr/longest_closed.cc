
// g++ $(pkg-config --cflags --libs opencv) longest_closed.cc -o longest_closed && ./longest_closed

#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <math.h>

#include <iostream>

using namespace cv;
using namespace std;

// http://stackoverflow.com/questions/6555629/algorithm-to-detect-longest_closed-of-paper-sheet-in-photo

// c/c++ dummy declaration
void cosine_longest ( std::vector < std::vector<cv::Point> > contours );
Mat angle_clusters( std::vector < std::vector<cv::Point> > contours, Mat_<float> &angles );
Mat coord_clusters( Size size, std::vector < std::vector<cv::Point> > contours, Mat_<float> angles);
// not in use
Point2f get_mass_center(Point a, Point b);
void get_closest_diagonal ( Rect rect,  Mat_<float> angles, std::vector<cv::Point> points, Mat &pic );

static float angle_2points ( cv::Point p1, cv::Point p2 ) {

  float ang = atan2(p1.y - p2.y, p1.x - p2.x);
  ang = ang * 180 / CV_PI;
  return (ang>0.0) ? ang : 180.0 + ang;
}

static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

// get the diagonal of the bounding rect...
double get_longest_side_poly( std::vector<cv::Point> approx ){

  Rect rect = boundingRect(approx);

  return sqrt(rect.width*rect.width + rect.height*rect.height );
}

int get_angles ( std::vector<cv::Point> approx, Mat drawing ) {

  // Number of vertices of polygonal curve
  int vtc = approx.size();

  // Get the degree (in cosines) of all corners
  // std::vector<double> cos;
  double ang, ang_deg;
  int has_angle90 = 0;
  std::vector<cv::Point> circles;
  for (int j = 1; j < vtc+1; j++) {

    ang = angle(approx[j%vtc], approx[j-2], approx[j-1]);
    // cos.push_back(ang);

    ang_deg = ang*180/CV_PI;

    if(ang_deg >-25 && ang_deg<25){
      // cv::circle( drawing, approx[j%vtc], 50,  cv::Scalar(0,0,255) );
      has_angle90 = 1;
      circles.push_back(approx[j-1]);
      // std::cout << "drawing circles... "  << std::endl;
    }

    // std::cout << "angle is: " << ang_deg << ", " << ang  << std::endl;
  }

  double diag = 0;
  has_angle90 && (diag = get_longest_side_poly ( circles ));

  if(has_angle90){

    std::cout << "diag is: " << diag << std::endl;

    if(diag>100){
      std::cout << "ok, drawing circles... " << std::endl;
      int clen = circles.size();
      for(int j=0; j<clen; ++j){

        cv::circle( drawing, circles[j], 50,  cv::Scalar(50,0,255) );
      }
    }

  }

  return 0;
}


void longest_closed()
{
   // Mat mat = imread( "./pics/heb.jpg");
   // Mat mat = imread( "./pics/heb2.jpg");
   // Mat mat = imread( "./pics/heb_new.jpg");
   // Mat mat = imread( "./pics/tj.jpg");
   Mat mat = imread( "./pics/tj2.jpg");


   cv::cvtColor(mat, mat, CV_BGR2GRAY);

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

   std::cout << "contours count: " <<  contours.size()  << std::endl;

   // TODO - go on from checking if the >10000 is a single len
   double len;
   std::vector< std::vector<cv::Point> > contours_f1;
   std::vector< std::vector<cv::Point> > contours_long;
   std::vector<std::vector<cv::Point> > contoursDraw (contours.size());
   Mat poly = Mat::zeros( mat.size(), CV_8UC3 );
   for (int i=0; i < (int)contours.size(); i++){

     cv::approxPolyDP(Mat(contours[i]), contoursDraw[i], 40, true);

     len = cv::arcLength(contours[i], true);

     if(len>0){
       std::cout << "closed line len...: " << len << std::endl;
       contours_f1.push_back(contours[i]);
       if(len>5000) contours_long.push_back(contoursDraw[i]);
     }
   }

   Mat drawing = Mat::zeros( mat.size(), CV_8UC3 );
   Mat clong = Mat::zeros( mat.size(), CV_8UC3 );
   cv::drawContours(drawing, contours_f1, -1, cv::Scalar(0,255,0),1);

   // cosine_longest(contours_long);
   // cosine_longest(contoursDraw);
   Mat_<float> angles, angles0, angles1;
   Mat_<int> labels = angle_clusters(contoursDraw, angles);
   std::cout << "angles ref: " << angles  << std::endl;

   std::vector< std::vector<cv::Point> > contours_l0;
   std::vector< std::vector<cv::Point> > contours_l1;

   for(int j=0; j<labels.rows; ++j){

     std::cout << "l.cols: " << labels(j,0) << std::endl;

     if(labels(j,0)==0){
       contours_l0.push_back(contoursDraw[j]);
       angles0.push_back(angles(j,0));
     }else if(labels(j,0)==1){
       contours_l1.push_back(contoursDraw[j]);
       angles1.push_back(angles(j,0));
     }
   }

   std::cout << "angles0: " << angles0 << ',' << "angles1: " << angles1 << std::endl;

   // TODO - dynamic here for 0,1 ...
   coord_clusters( mat.size(), contours_l0, angles0);

   // cv::drawContours(poly, contoursDraw, -1, cv::Scalar(0,255,0),1);
   cv::drawContours(poly, contours_l0, -1, cv::Scalar(0,255,0),1);
   cv::drawContours(clong, contours_long, -1, cv::Scalar(0,255,0),1);

   for(int i=0; i<(int)contours_long.size(); ++i){
     get_angles( contours_long[i], clong );
   }

   cv::imwrite( "./img_pre/long2.jpg", drawing);
   cv::imwrite( "./img_pre/long3.jpg", poly);
   cv::imwrite( "./img_pre/long4.jpg", clong);
}

void get_closest_diagonal ( Rect rect,  Mat_<float> angles, std::vector<cv::Point> points, Mat &pic ) {
  std::cout << "avg angles: " << mean(angles) << std::endl;

  // vx,vy,x,y
  // (vx, vy, x0, y0), where (vx, vy) is a normalized vector collinear to the line and (x0, y0) is a point on the line
  Vec4f line_result;
  fitLine(points, line_result, DIST_L2, 0, .01, .01 );

  float vx = line_result[0];
  float vy = line_result[1];
  float x = line_result[2];
  float y = line_result[3];

  float x0, y0, x1, y1;

  x0 = x - vx*x; x0<0 && (x0=0);
  y0 = y - vy*y; y0<0 && (y0=0);

  x1 = x + vx*pic.cols; x1>pic.cols && (x1=pic.cols);
  y1 = y + vy*pic.rows; y1>pic.rows && (y1=pic.rows);

  std::cout << "vec4f: " << line_result << ',' << "points: " << points << "line points" << Point(x0,y0)  << ',' << Point(x1,y1) << ',' << pic.cols << ',' << pic.rows  <<  std::endl;

  cv::line ( pic, Point(x0, y0), Point(x1, y1), cv::Scalar(0,64,255), 2, CV_AA );
}

Mat coord_clusters ( Size size, std::vector < std::vector<cv::Point> > contours, Mat_<float> angles ) {

  std::vector<cv::Point2f> points;
  for(int i=0; i<(int)contours.size(); ++i){
    points.push_back( Point2f(contours[i][0].x, 0) );
  }

  int clusterCount = 2, attempts = 1;
  Mat llabels, centers;
  kmeans(points, clusterCount, llabels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 100, 0.0001), attempts, KMEANS_PP_CENTERS, centers );
  std::cout << "\n\n ~~~~ coord clusters ~~~~ \n\n labels: " << llabels << "centers" << centers << "points" << points << std::endl;

  std::vector < std::vector<cv::Point> > contours_l0, contours_l1;
  Mat l0 = Mat::zeros( size, CV_8UC3 ), l1 = Mat::zeros( size, CV_8UC3 );

  Mat_<int> labels = llabels;  Mat_<float> angles0, angles1;
  for ( int j=0; j<labels.rows; ++j ) {
    if(labels(j,0)==0){
      contours_l0.push_back(contours[j]); angles0.push_back(angles(j,0));
    }else if(labels(j,0)==1){
      contours_l1.push_back(contours[j]); angles1.push_back(angles(j,0));
    }
  }

  cv::drawContours(l0, contours_l0, -1, cv::Scalar(0,255,0),1);  cv::drawContours(l1, contours_l1, -1, cv::Scalar(255,255,0),1);

  std::vector<cv::Point> points0, points1;
  for(int i=0; i<(int)contours_l0.size();++i){
    if ( contours_l0[i][0].x==0 || contours_l0[i][1].x==0 ) continue;
    points0.push_back(contours_l0[i][0]);
    points0.push_back(contours_l0[i][1]);
  }

  for(int i=0; i<(int)contours_l1.size();++i){
    if ( contours_l1[i][0].x==0 || contours_l1[i][1].x==0  ) continue;
    points1.push_back(contours_l1[i][0]);
    points1.push_back(contours_l1[i][1]);
  }

  std::cout << "points0:" << points0  << std::endl;  std::cout << "points1:" << points1  << std::endl;

  Rect r0 = cv::boundingRect(points0);  Rect r1 = cv::boundingRect(points1);
  get_closest_diagonal(r0, angles0, points0, l0);  get_closest_diagonal(r1, angles1, points1, l1);

  rectangle ( l0,r0,cv::Scalar(0,255,0) );  rectangle ( l1,r1,cv::Scalar(0,255,0) );

  cv::imwrite( "./img_pre/long5.jpg", l0);  cv::imwrite( "./img_pre/long6.jpg", l1);

  return labels;
}

Point2f get_mass_center(Point a, Point b){

  return Point2f( (int)((a.x+b.x)/2), (int)((a.y+b.y)/2) );
}

Mat angle_clusters( std::vector < std::vector<cv::Point> > contours, Mat_<float> &angles ){

  // Mat angles;
  for ( int i=0; i<(int)contours.size(); ++i ) {
    // angles.push_back(abs(angle_2points(contours[i][0], contours[i][1])));
    angles.push_back ( angle_2points(contours[i][0], contours[i][1]) );
  }

  int clusterCount = 2;
  Mat labels;
  int attempts = 5;
  Mat centers;
  kmeans(angles, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers );

  std::cout << "labels: " << labels << "centers" << centers << "angles" << angles << std::endl;

  return labels;
}

void cosine_longest ( std::vector < std::vector<cv::Point> > contours ) {

  Mat a,b;
  // double ab, aa, bb;
  int size_a, size_b;
  for ( int i=0; i<(int)contours.size(); ++i ) {

    if(contours[i].size()<2) continue;
    a = Mat(contours[i]);

    for(int j=0; j<(int)contours.size(); ++j){

      if(contours[j].size()<2) continue;

      size_a = (int)contours[i].size();
      size_b = (int)contours[j].size();

      if(size_a>size_b){
        for(int m=0; m<(size_a-size_b); ++m){
          contours[j].push_back(contours[j][size_b-1]);
        }
      }
      else if(size_b>size_a){
        for(int m=0; m<(size_b-size_a); ++m){
          contours[i].push_back(contours[i][size_a-1]);
        }
        // remake the matrix again...
        a = Mat(contours[i]);
      }

      b = Mat(contours[j]);

      // std::cout << "sizes: " << size_a << ',' << size_b << std::endl;
      // std::cout << "sizes: " << a << ',' << b << std::endl;
      std::cout << "angles: " << angle_2points(contours[i][0],contours[i][1]) << ',' <<  angle_2points(contours[j][0],contours[j][1]) << std::endl;

      // ab = a.dot(b);
      // aa = a.dot(a);
      // bb = b.dot(b);

      // std::cout << "cosine: "  <<  ab / (sqrt(aa) * sqrt(bb))  << std::endl;

      // std::cout << "cosine: " << cv::arcLength(contours[i], true) << ',' << cv::arcLength(contours[j], true) << ','  <<  ab / (sqrt(aa) * sqrt(bb))  << std::endl;

    }
  }
}

// TODO go on from doing another simple loop, just to calc the angles and then try the kmean clustering... yep! may the force be with you

int main ( int argc, char** argv )
{
  longest_closed();
  return 0;
}
