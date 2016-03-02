/*
*  ImageProcessing.cpp
*/
#include <jni.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc_c.h>

#include "multipleObjectTracking.hpp"

using namespace std;
using namespace cv;

Mat * mCanny = NULL;

// extern "C"
// http://stackoverflow.com/questions/22752555/need-an-advice-processing-color-image-from-camera-android-opencv-ndk-c
void convertYUV ( JNIEnv* env, int width, int height, jbyteArray &yuvArray, Mat &img ) {

     // Get the data from JEnv.
     jbyte * data = env->GetByteArrayElements(yuvArray, 0);

     // Convert to Mat object.
     Mat imgbuf( Size(width,height), CV_8UC1, (unsigned char*) data );
     // img = imdecode(imgbuf, CV_LOAD_IMAGE_COLOR);
     imgbuf = imdecode(imgbuf, CV_LOAD_IMAGE_GRAYSCALE);

    // Release the JNI data pointer.
    env->ReleaseByteArrayElements(yuvArray, data, 0);
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
