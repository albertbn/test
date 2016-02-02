
#include <iostream>
#include <fstream>
#include <ctime>

#include <opencv2/opencv.hpp>
#include "angle.hpp"
#include "close_center.hpp"
#include "cluster.hpp"
#include "common.hpp"
#include "main_geometry.hpp"
#include "point.hpp"
#include "static_fields.hpp"

#include "ocr/main.hpp"

using namespace cv;
using namespace std;

// LOGD ( "should have written 2 images 0,1, yep? " );

// start here
void longest_closed ( Mat& mat ) {

#ifdef ANDROID
  LOGD ( "longest_closed mat (width, height): %d, %d \n", mat.size().width, mat.size().height );
#endif // ANDROID

  outfile << "longest_closed start: " << clock_ticks_to_ms(clock()-clock_start) << endl; clock_start=clock();

  size_mat = mat.size();

  cv::cvtColor ( mat, mat, CV_BGR2GRAY );

  cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Point(19,19));
  cv::Mat dilated;
  cv::dilate(mat, dilated, kernel);
  kernel.release();

  outfile << "longest_closed after kernel release: " <<  clock_ticks_to_ms(clock()-clock_start) << endl; clock_start=clock();

  blur ( dilated, dilated, Size(10,10) );

// #ifndef ANDROID
  cv::imwrite( path_img+"/long0.jpg", dilated ); /*boost performance*/
// #endif // ANDROID
  outfile << "longest_closed after blur1: " <<  clock_ticks_to_ms(clock()-clock_start) << endl; clock_start=clock();

  cv::Mat edges;
  cv::Canny(dilated, edges, 40, 1);
  dilated.release();
  blur(edges, edges, Size(10,10));

  outfile << "longest_closed after blur2: " <<  clock_ticks_to_ms(clock()-clock_start) << endl; clock_start=clock();

// #ifndef ANDROID
  cv::imwrite( path_img+"/long1.jpg", edges); /*boost performance*/
// #endif // ANDROID

  std::vector< std::vector<cv::Point> > contours;
  cv::findContours(edges, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS);
  edges.release();

  outfile << "longest_closed after find contours: " <<  clock_ticks_to_ms(clock()-clock_start) << endl; clock_start=clock();

  double len;
  std::vector<double> len_contours_contoursDraw2, len_contours_closed;
  std::vector< std::vector<cv::Point> > contours_f1;
  std::vector< std::vector<cv::Point> > contours_long, contours_medium;
  std::vector<std::vector<cv::Point> > contoursDraw(contours.size());
  std::vector<std::vector<cv::Point> > contoursDraw2;

// #ifndef ANDROID
  Mat poly = Mat::zeros ( size_mat, CV_8UC3 ) ; /*boost performance*/
// #endif // ANDROID

  double min_line_length = MIN_LINE_LENGTH_CONSIDERED_SIDE = max(size_mat.width, size_mat.height)/13.0; /*TODO - here - check this chap*/
  MIN_LINE_LENGTH_CONSIDERED_SIDE*=4.5; /*this is a TODO for sure - should implement some other algo for \/  / clustered in vert - pics/18.jpg*/
  int min_closed_line_len = (size_mat.width + size_mat.height);

  // fills contoursDraw2 :: filters out lines shorter than 200 px, straightens lines with approxPoly to contoursDraw(2), pushes to contours_long if > 5000 px..
  outfile << "longest_closed after convert, kernel, blur, canny, contours took: " <<  clock_ticks_to_ms(clock()-clock_start) << endl; clock_start=clock();

  for ( int i=0; i < (int)contours.size(); i++ ) {

    len = cv::arcLength(contours[i], true);
    if ( len < min_line_length ) {
      continue;
    }
    len_contours_contoursDraw2.push_back ( len );

    cv::approxPolyDP(Mat(contours[i]), contoursDraw[i], 40, true);
    contoursDraw2.push_back(contoursDraw[i]);

    if ( len>0 ) {
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
// #ifndef ANDROID
  Mat drawing = Mat::zeros( size_mat, CV_8UC3 ); /*boost performance*/
// #endif // ANDROID
  Mat clong = Mat::zeros( size_mat, CV_8UC3 );
// #ifdef ANDROID
  // clong.release(); /*boost performance*/
// #endif // ANDROID
// #ifndef ANDROID
  cv::drawContours(drawing, contours_f1, -1, cv::Scalar(0,255,0),1);
// #endif // ANDROID

  int _angle90_count=0; std::vector<cv::Point> points4;
  // count the ~90 degree angles...
  for ( int i=0; i<(int)contours_long.size(); ++i ) {
    _angle90_count += get_angle_approx90_count ( contours_long[i], clong, points4/*ref*/ );
  }

  // DONE, yep! - somewhere here start and implement the persp.cc - good luck - calc center, order points, etc...
  std::cout << " \t\t ~~~ ``` _angle90_count:" << _angle90_count << std::endl;
  // OK, this is the dotted line connection and expansion algorithm
  if ( _angle90_count!=4 || !corners_magick_do(size_mat, points4 /*a 4 point chap - validate this folk*/) ) {

    // DONE - add logic here for using just longest and parts... for cases where there is longest and at least 1 90 deg angle...
    if ( contours_long.size() || contours_medium.size() ){
      std::cout << " \t\t contours_long.size() || contours_medium.size " << contours_long.size() << ',' << contours_medium.size() << std::endl;
      // std::cout << "contours long || medium" << std::endl;
      for ( int i=0; i<(int)contours_long.size(); ++i ) { contours_medium.push_back(contours_long[i]); }
      deal_with_geometry_when_not_enough_90d_angles( size_mat, contours_medium, len_contours_closed, min_line_length);
    }
    else {
      std::cout << " \t\t ELSE contours_long.size() || contours_medium.size( " << std::endl;
      deal_with_geometry_when_not_enough_90d_angles( size_mat, contoursDraw2, len_contours_contoursDraw2, min_line_length);
    }

    // if ( lines4intersect.size()<4 ) {
    std::cout << "intersecting man..." << "\npoints4: " << points4 << "\nlines4intersect: " << Mat(lines4intersect)  << std::endl;
    points4.clear();
    intersect_n_get_points ( points4 /*ref*/ );
    std::cout << "\npoints4 after intersect..." << points4 << std::endl;
    corners_magick_do( size_mat, points4 /*a 4 point chap - validate this folk*/);
    // }
    final_magic_crop_rotate ( mat, points4 /*ref*/ );

    std::cout << "lines4intersect size: " << lines4intersect.size() << ",\n points4: " << points4 << std::endl;
  }
  else {
    final_magic_crop_rotate (  mat, points4 /*ref*/ );
  }

  // std::cout << "contoursDraw2: " << Mat(contoursDraw2) << ",\n contours_long: " << Mat(contours_long) << std::endl;

// #ifndef ANDROID
  if(contoursDraw2.size())
    cv::drawContours(poly, contoursDraw2, -1, cv::Scalar(0,255,0),1); /*boost performance*/
  if(contours_long.size())
    cv::drawContours(clong, contours_long, -1, cv::Scalar(0,255,0),1); /*boost performance*/

  cv::imwrite( path_img + "/long2.jpg", drawing); /*boost performance*/
  cv::imwrite( path_img + "/long3.jpg", poly); /*boost performance*/
  cv::imwrite( path_img + "/long4.jpg", clong); /*boost performance*/
// #endif // ANDROID

  mat.release();
}

void deal_with_geometry_when_not_enough_90d_angles (
                                                          Size mat_size,
                                                          std::vector<std::vector<cv::Point> > contoursDraw2,
                                                          std::vector<double> len_contours_contoursDraw2,
                                                          double min_line_length
                                                          ) {
  // shall we work? - well :) maybe - c u next time :) suck Shawn, suck
  split_contours_2_dotted_lines ( /*ref*/contoursDraw2, /*ref*/len_contours_contoursDraw2, min_line_length );

  Mat_<float> angles, angles0, angles1;
  Mat_<double> angle_centers;
  Mat_<int> labels = angle_clusters(contoursDraw2, angles, angle_centers); /*OK if a single line,*/

  std::vector< std::vector<cv::Point> > contours_l0;
  std::vector< std::vector<cv::Point> > contours_l1;
  double len_sum0=0.0, len_sum1=0.0;
  std::vector<double> len_contours0, len_contours1;

  /*separate / divide into 2 groups with approximate 90 degree alignment */
  for ( int j=0; j<labels.rows; ++j ) {

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

void split_contours_2_dotted_lines( std::vector<std::vector<cv::Point> > &contoursDraw2, std::vector<double> &len_contours_contoursDraw2, double min_line_length ) {

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

void final_magic_crop_rotate ( Mat mat,  std::vector<cv::Point>& points4 ) {

  outfile << "start of final magic crop: " <<  clock_ticks_to_ms(clock()-clock_start) << endl; clock_start=clock();

// #ifndef ANDROID
  Mat mb;
  if ( file_exists(path_img + "/long7.jpg") ) /*boost performance*/
    mb = imread ( path_img + "/long7.jpg" ); /*boost performance*/
  else /*boost performance*/
    mb = Mat::zeros ( mat.size(), CV_8UC3 ); /*boost performance*/
// #endif // ANDROID

  if ( points4.size()>4 ) { /*for sort points - draw all circles*/
// #ifndef ANDROID
    for ( int i=0; i<(int)points4.size(); ++i ) { /*boost performance*/
      cv::circle ( mb, points4[i], 50, cv::Scalar(150,55,70) ); /*boost performance*/
    }
// #endif // ANDROID
    sort_points_closest_2center(points4); /*boost performance*/
  }

  std::vector<cv::Point2f> points4f;
  // this here is probably closest to the size of the original invoice... well, let's try... tension :)
  cv::RotatedRect rect_minAreaRect = minAreaRect(points4);
// #ifndef ANDROID
  RNG rng(12345); /*boost performance*/
  Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) ); /*boost performance*/
// #endif // ANDROID

  Point2f rect_points[4]; rect_minAreaRect.points( rect_points );

  for ( int i=0; i<(int)points4.size(/*4*/); ++i ) {
    points4f.push_back(points4[i]);
// #ifndef ANDROID
    line( mb, rect_points[i], rect_points[(i+1)%4], color, 1, 8 ); /*boost performance*/
    cv::circle ( mb, points4[i], 50, cv::Scalar(50,0,255) ); /*boost performance*/
// #endif // ANDROID
  }

  bool is_mat_width = size_mat.width>size_mat.height; /*is width larger*/
  int small = min(rect_minAreaRect.size.width, rect_minAreaRect.size.height);
  int large = max(rect_minAreaRect.size.width, rect_minAreaRect.size.height);
  !is_mat_width && (small=small^large) && (large=small^large) && (small=small^large); /*XOR swap*/
  // cv::Mat quad = cv::Mat::zeros ( small, large, CV_8UC3 );
  cv::Mat quad = cv::Mat::zeros ( small, large, CV_8U );

  std::vector<cv::Point2f> quad_pts;
  quad_pts.push_back(cv::Point2f(0, 0));
  quad_pts.push_back(cv::Point2f(quad.cols, 0));
  quad_pts.push_back(cv::Point2f(quad.cols, quad.rows));
  quad_pts.push_back(cv::Point2f(0, quad.rows));

  if ( points4f.size()==4 ) {
    cv::Mat transmtx = cv::getPerspectiveTransform ( points4f, quad_pts );
    cv::warpPerspective ( mat, quad, transmtx, quad.size() );

    outfile << "before ocr do it in final_magic: " <<  clock_ticks_to_ms(clock()-clock_start) << endl; clock_start=clock();
    // ocr_doit ( quad /* send by reference to OCR */ );
    outfile << "after ocr do it: " <<  clock_ticks_to_ms(clock()-clock_start) << endl; clock_start=clock();
  }
  else {
    // std::cout << "checking points4f... " << points4f << std::endl;
    outfile << "no ocr executed - else case in final_magic: " <<  clock_ticks_to_ms(clock()-clock_start) << endl; clock_start=clock();
  }


// #ifndef ANDROID
  cv::imwrite ( path_img + "/long7.jpg", mb ); /*boost performance*/
  cv::imwrite ( path_img + "/long8.jpg", quad );
// #endif // ANDROID

}
