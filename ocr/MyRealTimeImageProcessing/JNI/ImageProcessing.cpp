/*
*  ImageProcessing.cpp
*/
#include <jni.h>

#include <fstream>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc_c.h>

#include "ocr/tess.hpp" /*used for rotate - rot90*/
#include "static_fields.hpp"
#include "multipleObjectTracking.hpp"
#include "converters.hpp"

using namespace std;
using namespace cv;

ofstream outfile;
ofstream outfile_ocr;
string path_sd_card;
string IMG_PATH;

int width_preview;
int get_width_preview(){
  return width_preview;
}
void set_width_preview(int value){
  width_preview = value;
}

int height_preview;
int get_height_preview(){
  return height_preview;
}
void set_height_preview ( int value ) {
  // height_preview = value*1.5; /*! don't *1.5 - comes already so!!!???*/
  height_preview = value; /*! don't *1.5 - comes already so!!!???*/
}

vector < vector<Point> > contours_poly2; /*this is a static filed, that could be accessed from outside???*/

void fn_transform_point ( Point& point ) {

  // double x(point.x); // point.x = height-point.y; // point.y = x;

  point.x^=point.y; point.y^=point.x; point.x^=point.y; /*XOR swap*/
  point.x = get_height_preview()-point.x;
}

void fn_transform_vec_point ( vector<Point>& vec_point ) {
  for_each (vec_point.begin(), vec_point.end(), fn_transform_point);
}

void rotate_contours_90 ( vector < vector<Point> >& contours_rotate ) {
  for_each ( contours_rotate.begin(), contours_rotate.end(), fn_transform_vec_point );
}

// 2016-06-14, this folk processes the photo taken, crops, ocr etc. the crop is based on the each-frame-colour-detected contours
extern "C"
jboolean
Java_my_project_MyRealTimeImageProcessing_ImgProcessOcr_saveMiddleClass ( /*TODO - confirm this works*/
// Java_my_project_MyRealTimeImageProcessing_MyRealTimeImageProcessing_saveMiddleClass (
                JNIEnv* env, jobject
                ,jstring jroot_folder_path
                ,jstring jimg_unique_no_ext
                ,jlong matAddr
 ) {

  string img_unique_no_ext = env->GetStringUTFChars(jimg_unique_no_ext, 0);

  path_sd_card = env->GetStringUTFChars(jroot_folder_path, 0); /* doesn't end with / */;
  IMG_PATH = path_sd_card + "/tessdata/img/" + img_unique_no_ext + ".jpg";
  // IMG_PATH = path_sd_card + "/tessdata/img/" + img_unique_no_ext + ".png";
  string path_dump = path_sd_card + "/tessdata/dump.txt";
  string path_ocr = path_sd_card + "/tessdata/ocr.txt";

  remove ( IMG_PATH.c_str() ) ;
  remove ( path_dump.c_str() ) ;
  remove ( path_ocr.c_str() ) ;

  //open streams
  outfile.open ( path_dump.c_str(), ios_base::app | ios_base::in  );
  outfile_ocr.open ( path_ocr.c_str(), ios_base::app | ios_base::in );

  Mat mat = *( (Mat*)matAddr );
  save_middle_class ( mat );

  outfile_ocr.close();
  outfile.close();

  env->ReleaseStringUTFChars(jroot_folder_path, path_sd_card.c_str());
  env->ReleaseStringUTFChars(jimg_unique_no_ext, img_unique_no_ext.c_str());

  return true;
}

// do_frame ( mFrame );
// new hope, go on from here: https://github.com/MasteringOpenCV/code/blob/master/Chapter1_AndroidCartoonifier/Cartoonifier_Android/jni/jni_part.cpp
// go on from - passing vec_vec_point back and forth to Java and JNI, yep?:
// http://stackoverflow.com/questions/22319168/opencv-java-code-pass-point-object-to-native-codec
extern "C"
jboolean
Java_my_project_MyRealTimeImageProcessing_CameraPreview_colourDetect (
                JNIEnv* env, jobject,
                jint width, jint height,
                jbyteArray yuv, jintArray bgra,
                jlong jout_vec_vec_point,
                jstring jroot_folder_path
                ,jintArray hsv6
                                                                      ) {

  // delete[] jroot_folder_path;

  // string root_folder_path; /* doesn't end with / */
  // root_folder_path = env->GetStringUTFChars(jroot_folder_path, 0); /*yep! pointer*/
  // // root_folder_path = root_folder_path + "/tessdata/img/" + (++COLOUR_FRAME_COUNT) + ".jpg";
  // root_folder_path = root_folder_path + "/tessdata/img/dump.txt";
  // outfile.open ( root_folder_path.c_str(), ios_base::app );

  set_width_preview((int)width);
  set_height_preview((int)height);

  // outfile << "width: " << width << '\n';
  // outfile << "height: " << height << '\n';
  // outfile << "width_preview: " << get_width_preview() << '\n';
  // outfile << "height_preview: " << get_height_preview() << '\n' ;

  // Get native access to the given Java arrays.
  jbyte* _yuv  = env->GetByteArrayElements(yuv, 0);
  jint*  _bgra = env->GetIntArrayElements(bgra, 0);
  jint*  _hsv6 = env->GetIntArrayElements(hsv6, 0);

  // Prepare a cv::Mat that points to the YUV420sp data.
  Mat myuv ( height*1.5, width, CV_8UC1, (uchar *)_yuv ); /*orig*/
  Mat mbgra ( height, width, CV_8UC4, (uchar *)_bgra ); /*orig*/

  // Convert the color format from the camera's NV21 "YUV420sp" format to an Android BGRA color image.
  cvtColor ( myuv, mbgra, CV_YUV420sp2BGRA ); /*UNMARK*/

  // rot90 ( mbgra, 1 ); /*TODO - do it dynamic*/

  // OpenCV can now access/modify the BGRA image if we want ...
  do_frame ( mbgra, Scalar(_hsv6[0], _hsv6[1], _hsv6[2]), Scalar(_hsv6[3],_hsv6[4],_hsv6[5]) ); /*UNMARK*/
  // do_frame ( mbgra, Scalar(0,0,200),  Scalar(10,10,255) ); /*UNMARK*/

  vector < vector<Point> > contours_rotated ( contours_poly2 );
  rotate_contours_90(contours_rotated); /*since app is in portrait mode - TODO - maybe do a dynamic check here for exif/orient*/

  cv::Mat& mat_out_vec_vec_point = *(cv::Mat*) jout_vec_vec_point;
  // vector_vector_Point_to_Mat ( contours_poly2, mat_out_vec_vec_point);
  vector_vector_Point_to_Mat ( contours_rotated, mat_out_vec_vec_point );

  // outfile.close();
  // (*env).ReleaseStringUTFChars(jroot_folder_path, root_folder_path.c_str());

  // Release the native lock we placed on the Java arrays.
  env->ReleaseIntArrayElements(bgra, _bgra, 0);
  env->ReleaseByteArrayElements(yuv, _yuv, 0);
  env->ReleaseIntArrayElements(hsv6, _hsv6, 0);

  return true;
}
