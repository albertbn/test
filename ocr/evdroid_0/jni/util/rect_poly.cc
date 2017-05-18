
#include <opencv2/opencv.hpp>
#include "angle.hpp"
#include "point.hpp"
#include "static_fields.hpp"

using namespace cv;
using namespace std;

//my father was born 1938, my mother in  1949
const float DEVIATION_RATIO = .038; /* (if > then suspect) usually, if the stdev is smaller than mean, then if stdev/mean > DEVIATION_RATIO - suspect is set*/
const float DEVIATION_MIN_RATIO_ALLOWED_ANGLE = .49; /* (if < then suspect) values less than that are rebel suspects, greater than that - close to 1.0 are OK */

double get_longest_side_poly ( std::vector<cv::Point> approx ) {
  Rect rect = boundingRect(approx);
  return sqrt ( rect.width*rect.width + rect.height*rect.height );
}

//severity
void find_rebel_suspect ( Mat_<float> m_one_dimension, vector<int> &suspect_indexes, vector<float> &severity ) {

  suspect_indexes.clear();
  severity.clear();
  cv::Scalar mean, stdev;
  cv::meanStdDev( m_one_dimension, mean/*ref*/, stdev/*ref*/ );

  cout << "find_rebel_suspect :: mat: " << m_one_dimension << endl;

  float small, large, ssmall, slarge;
  double severe;
  ssmall = min(mean.val[0], stdev.val[0]);
  slarge = max(mean.val[0], stdev.val[0]);

  if ( (ssmall/slarge)>DEVIATION_RATIO ) {
    for ( int i=0; i<m_one_dimension.rows; ++i ) {
      small = abs(mean.val[0]-stdev.val[0]);
      large = mean.val[0]+stdev.val[0];
      // if not in standard deviation range - add to suspect list
      if ( !(small<m_one_dimension[i][0] &&  m_one_dimension[i][0]<large) ) {
        suspect_indexes.push_back(i);

        severe = abs ( m_one_dimension[i][0]-mean.val[0] ) ;
        small = min ( severe, stdev.val[0] ) ;
        large = max ( severe, stdev.val[0] ) ;
        severity.push_back ( small/large ) ;
      }
      else {
        severity.push_back ( 0.0 ) ; /*!*/
      }
    }
  }

  cout << "find_rebel_suspect :: mean, stdev: " << mean << ',' << stdev << endl;
}

vector<cv::Point> get_points_from_contours (
                                            bool is_vert,
                                            vector< std::vector<cv::Point> > &contours,
                                            Mat_<float> &angles,
                                            vector<double> &len_contours
                                            ) {
  vector<float> vxy;
  float xalc;
  for ( int i=0; i<(int)contours.size(); ++i ) {
    xalc = (is_vert)
      ? (contours[i][0].x + contours[i][1].x)/2.0
      : (contours[i][0].y + contours[i][1].y)/2.0;
    vxy.push_back(xalc);
  }

  vector<int> suspect_indexes_xy, suspect_indexes_angles;
  vector<float> severe_suspect_indexes_xy, severe_suspect_indexes_angles;
  find_rebel_suspect ( Mat_<float>(vxy), suspect_indexes_xy, severe_suspect_indexes_xy );
  find_rebel_suspect ( angles, suspect_indexes_angles, severe_suspect_indexes_angles );

  cout << "get_points_from_contours :: rebels: " << Mat(suspect_indexes_xy) << ',' << Mat(suspect_indexes_angles) << ',' << Mat(severe_suspect_indexes_angles) << endl;
  //ON IT - filter out lines by vertical/horizontal out of standard deviation
  //for horizontal - calculate avg y, for vert avg x
  //multiply by angle
  //then add *len times to regard also the len
  //then do standard deviation and clear noise
  vector<cv::Point> points;
  Mat_<float> angles2;
  vector<double> len_contours2;

  for ( int i=0; i<(int)contours.size(); ++i ) {
    if ( contours[i][0].x==0 || contours[i][1].x==0  ) continue;

    if ( suspect_indexes_xy.size() || suspect_indexes_angles.size() ){

      if (
          (find(suspect_indexes_xy.begin(), suspect_indexes_xy.end(), i) != suspect_indexes_xy.end())
          &&
          (find(suspect_indexes_angles.begin(), suspect_indexes_angles.end(), i) != suspect_indexes_angles.end())
          )
        continue;

      else if(
              ( find(suspect_indexes_angles.begin(), suspect_indexes_angles.end(), i) != suspect_indexes_angles.end() )
              ){

        cout << "~~~DEVIATION_RATIO_ANGLE severe, check, sever_vec~~~ :" << severe_suspect_indexes_angles[i] << ',' << (severe_suspect_indexes_angles[i]<DEVIATION_MIN_RATIO_ALLOWED_ANGLE) << ',' << Mat(severe_suspect_indexes_angles) << endl;

        if ( severe_suspect_indexes_angles[i] < DEVIATION_MIN_RATIO_ALLOWED_ANGLE ) {
          cout << "yep???" << ',' << severe_suspect_indexes_angles[i] << endl;
          continue;
        }
      }
    }

    points.push_back(contours[i][0]);
    points.push_back(contours[i][1]);
    angles2.push_back(angles[i][0]);
    len_contours2.push_back(len_contours[i]);
  }

  angles = angles2;
  return points;
}

float get_angle_avg_by_lengths ( Mat_<float> angles,  vector<double> len_contours ) {

  float angles_sum_by_len = 0.0;
  double divide_by=0;
  for ( int i=0; i<angles.rows; ++i ) {
    angles_sum_by_len+=angles[i][0]*len_contours[i];
    divide_by+=len_contours[i];
  }

  return angles_sum_by_len/divide_by;
}

void get_geometry_points_vertical ( float angle_avg_by_len, Point &up, Point &down, float x_fitline, float y_fitline ) {

  float x_upper=0, x_lower=0;
  float ang = abs ( angle_avg_by_len-90.0 ), ang_tan; /* the smaller angle (close to 1 degrees) */
  cout << "get_geometry_points_vertical :: ang degrees: " << ang << endl;
  ang = abs(ang*CV_PI/180.0); /* to rad */
  cout << "get_geometry_points_vertical :: ang rad: " << ang << endl;

  ang_tan = tan(ang);
  x_upper = y_fitline * tan(ang_tan);
  x_lower = (size_mat.height-y_fitline) * tan(ang_tan);

  // this decides in which direction the line is tilted
  if ( angle_avg_by_len<90 /*90, 45*/ ) {
    x_upper += x_fitline;
    x_lower = x_fitline - x_lower;
  }
  else { /*90, 135*/
    x_upper = x_fitline-x_upper;
    x_lower += x_fitline;
  }

  up = Point(x_upper, 0);
  down = Point(x_lower, size_mat.height);

  cout << "get_geometry_points_vertical :: DIY upper, lower :" << up << ',' << down << endl;
}

void get_geometry_points_horizontal ( float angle_avg_by_len, Point &left, Point &right, float x_fitline, float y_fitline ) {

  float y_left=0, y_right=0;
  float ang = abs ( 180 - angle_avg_by_len ), ang_tan; /* the smaller angle (close to 1 degrees) */
  cout << "get_geometry_points_horizontal :: ang degrees: " << ang << endl;
  ang = abs(ang*CV_PI/180.0); /* to rad */
  cout << "get_geometry_points_horizontal :: ang rad: " << ang << endl;

  ang_tan = tan(ang);
  y_left = x_fitline * ang_tan;
  y_right = (size_mat.width-x_fitline) * ang_tan;

  // this decides in which direction the line is tilted
  if ( angle_avg_by_len>180 ) {
    y_left += y_fitline;
    y_right = y_fitline - y_right;
  }
  else {
    y_left = y_fitline-y_left;
    y_right += y_fitline;
  }

  left = Point ( 0, y_left );
  right = Point( size_mat.width, y_right );

  cout << "get_geometry_points_horizontal :: DIY left, right :" << left << ',' << right << endl;
}

// something stinky here? 6 Feb 2016
void get_closest_diagonal ( Mat_<float> angles, vector< std::vector<cv::Point> > contours, Mat &pic, vector<double> len_contours ) {

  float angle_avg = mean(angles)[0]; /*calculate, to know if horizontal or vertical*/
  bool is_vert = is_vertical(angle_avg);

  vector<cv::Point> points = get_points_from_contours ( is_vert, contours/*ref*/, angles /*ref*/, len_contours /*ref*/ );

  cout << "\n\n=========\nget_closest_diagonal :: angles, points\n" << '\n' << angles << '\n' << points << endl;
  cout << "get_closest_diagonal :: angle_avg, is_vert: " << angle_avg << ',' << is_vert << endl;

  // vx,vy,x,y
  // (vx, vy, x0, y0), where (vx, vy) is a normalized vector collinear to the line and (x0, y0) is a point on the line
  Vec4f line_result;
  fitLine ( points, line_result, CV_DIST_WELSCH, 0, .01, .01 );

  float vx = line_result[0];
  float vy = line_result[1];
  float x = line_result[2];
  float y = line_result[3];

  cv::circle ( pic, Point(x,y), 50, cv::Scalar(255,255,255) );

  float angle_avg_by_len = angle_avg = get_angle_avg_by_lengths ( angles, len_contours );
  Point p1, p2;
  // float angle_avg_by_len =  angle_avg;
  // DONE - go on from x = 1291 * atan2(180-angle... vert hor rad - see get_max_deviation in common)
  if ( is_vert ) {
    get_geometry_points_vertical ( angle_avg_by_len, p1, p2, x, y );
  }
  else {
    get_geometry_points_horizontal ( angle_avg_by_len, p1, p2, x, y );
  }

  cv::circle ( pic, p1, 50, cv::Scalar(255,255,255) );
  cv::circle ( pic, p2, 50, cv::Scalar(255,255,255) );

  cout << "get_closest_diagonal :: angle_avg_by_len : " << angle_avg_by_len << endl;
  cout << "get_closest_diagonal :: vx, vy, x, y : " << vx << ',' << vy << ',' << x << ',' << y << endl;

  float x0, y0, x1, y1;

  // float larger = max(size_mat.width, size_mat.height);

  // if ( is_vert && angle_avg<=90 )
  //   x0 = x + vx*1.2*larger;
  // else
  //   x0 = x - vx*1.2*larger;

  // // ON IT - make the same for horizontal with y
  // if ( !is_vert && angle_avg>=180 )
  //   y0 = y + vy*1.2*larger;
  // else {
  //   y0 = y - vy*1.2*larger;
  // }

  // if ( is_vert && angle_avg<=90 )
  //   x1 = x - vx*1.2*larger;
  // else
  //   x1 = x + vx*1.2*larger;

  // // ON IT - make the same for horizontal with y
  // if ( !is_vert && angle_avg>=180 )
  //   y1 = y - vy*1.2*larger;
  // else
  //   y1 = y + vy*1.2*larger;

  float angle_candidate_fitLine;
  // angle_2points ( Point(x0,y0), Point(x1,y1), angle_candidate_fitLine /*ref*/ ) ;
  // cout << "angle_candidate_fitLine :: " << angle_candidate_fitLine << endl;

  // get the closest angle candidate to angle fitLine
  if (
      1==1 ||
      //vert
      (is_vert && angle_candidate_fitLine>90.0 && angle_candidate_fitLine>angle_avg_by_len)
      ||
      //vert
      (is_vert && angle_candidate_fitLine<90.0 && angle_candidate_fitLine<angle_avg_by_len)
      ||
      //hor
      (!is_vert && angle_candidate_fitLine>180.0 && angle_candidate_fitLine>angle_avg_by_len)
      ||
      //hor
      (!is_vert && angle_candidate_fitLine<180.0 && angle_candidate_fitLine<angle_avg_by_len)
      ) {
    cout << "get_closest_diagonal :: C H A N G E D  to pi, p2" << endl;
    x0=p1.x; y0 = p1.y;
    x1=p2.x; y1 = p2.y; /*!*/
  }

  // cv.Line(img, (x0-m*vx[0], y0-m*vy[0]), (x0+m*vx[0], y0+m*vy[0]), (0,0,0))


  cout << "get_closest_diagonal :: x0, y0, x1, y1 : " << x0 << ',' << y0 << ',' << x1 << ',' << y1  << endl;
  if ( lines4intersect_validate( is_vertical(angle_avg), Point(x,y), Vec4i(x0,y0,x1,y1) ) ) {
// #ifndef ANDROID
    cv::line ( pic, Point(x0, y0), Point(x1, y1), cv::Scalar(0,64,255), 2, CV_AA ); /*boost performance*/
// #endif // ANDROID
  }
}