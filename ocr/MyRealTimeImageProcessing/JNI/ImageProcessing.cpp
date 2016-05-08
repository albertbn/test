/*
*  ImageProcessing.cpp
*/
#include <jni.h>

#include <fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc_c.h>

#include "static_fields.hpp"
#include "multipleObjectTracking.hpp"

using namespace std;
using namespace cv;

ofstream outfile;
ofstream outfile_ocr;
string path_sd_card;
string IMG_PATH;

Mat * mCanny = NULL; /*not used for now*/

extern "C"
jboolean
Java_my_project_MyRealTimeImageProcessing_MyRealTimeImageProcessing_saveMiddleClass (
                JNIEnv* env, jobject,
                jstring jroot_folder_path,
                jstring jimg_unique_no_ext,
                jlong matAddr ) {

  string root_folder_path; /* doesn't end with / */
  string img_unique_no_ext;

  root_folder_path = (*env).GetStringUTFChars(jroot_folder_path, 0);
  img_unique_no_ext = (*env).GetStringUTFChars(jimg_unique_no_ext, 0);

  path_sd_card = root_folder_path;
  // IMG_PATH = root_folder_path + "/tessdata/img/" + img_unique_no_ext + ".jpg";
  IMG_PATH = root_folder_path + "/tessdata/img/" + img_unique_no_ext + ".png";
  string path_dump = root_folder_path + "/tessdata/dump.txt";
  string path_ocr = root_folder_path + "/tessdata/ocr.txt";

  //  remove ( IMG_PATH.c_str() ) ;
  remove ( path_dump.c_str() ) ;
  remove ( path_ocr.c_str() ) ;

  //open streams
  outfile.open ( path_dump.c_str(), ios_base::app );
  outfile_ocr.open ( path_ocr.c_str(), ios_base::app );

  Mat mat = *((Mat*)matAddr);

  save_middle_class ( mat );

  outfile_ocr.close();
  outfile.close();

  // (*env).ReleaseStringUTFChars(jroot_folder_path, root_folder_path.c_str());
  // (*env).ReleaseStringUTFChars(jimg_unique_no_ext, img_unique_no_ext.c_str());

  return true;
}

// do_frame ( mFrame );
// new hope, go on from here: https://github.com/MasteringOpenCV/code/blob/master/Chapter1_AndroidCartoonifier/Cartoonifier_Android/jni/jni_part.cpp
extern "C"
jboolean
Java_my_project_MyRealTimeImageProcessing_CameraPreview_colourDetect (
                JNIEnv* env, jobject,
                jint width, jint height, jbyteArray yuv, jintArray bgra ) {

  // Get native access to the given Java arrays.
  jbyte* _yuv  = env->GetByteArrayElements(yuv, 0);
  jint*  _bgra = env->GetIntArrayElements(bgra, 0);

  // Prepare a cv::Mat that points to the YUV420sp data.
  Mat myuv(height + height/2, width, CV_8UC1, (uchar *)_yuv);
  // Prepare a cv::Mat that points to the BGRA output data.
  Mat mbgra(height, width, CV_8UC4, (uchar *)_bgra);

  // Convert the color format from the camera's
  // NV21 "YUV420sp" format to an Android BGRA color image.
  cvtColor(myuv, mbgra, CV_YUV420sp2BGRA);

  // OpenCV can now access/modify the BGRA image if we want ...
  // cv::circle ( mbgra, Point(200,200), 70, cv::Scalar(255,255,255) ) ;
  do_frame ( mbgra );

  // Release the native lock we placed on the Java arrays.
  env->ReleaseIntArrayElements(bgra, _bgra, 0);
  env->ReleaseByteArrayElements(yuv, _yuv, 0);

  return true;
}

extern "C"
jboolean
Java_my_project_MyRealTimeImageProcessing_CameraPreview_ImageProcessing (
                JNIEnv* env, jobject thiz,
                jint width, jint height,
                jbyteArray NV21FrameData, jintArray outPixels) {

        jbyte * pNV21FrameData = env->GetByteArrayElements(NV21FrameData, 0);
        jint * poutPixels = env->GetIntArrayElements(outPixels, 0);

        if ( mCanny == NULL )
        {
                mCanny = new Mat(height, width, CV_8UC1);
        }

        Mat mGray(height, width, CV_8UC1, (unsigned char *)pNV21FrameData);
        Mat mResult(height, width, CV_8UC4, (unsigned char *)poutPixels);

        IplImage srcImg = mGray;
        IplImage CannyImg = *mCanny;
        IplImage ResultImg = mResult;

        cvCanny(&srcImg, &CannyImg, 80, 100, 3);
        cvCvtColor(&CannyImg, &ResultImg, CV_GRAY2BGRA);

        env->ReleaseByteArrayElements(NV21FrameData, pNV21FrameData, 0);
        env->ReleaseIntArrayElements(outPixels, poutPixels, 0);

        return true;
}
