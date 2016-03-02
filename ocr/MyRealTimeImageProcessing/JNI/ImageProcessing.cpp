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
		JNIEnv* env, jobject thiz,
		jint width, jint height,
		jbyteArray NV21FrameData,
                jintArray outPixels ) {

  jbyte * pNV21FrameData = env->GetByteArrayElements(NV21FrameData, 0);
  // signed char * pNV21FrameData = env->GetByteArrayElements(NV21FrameData, 0);
  jint * poutPixels = env->GetIntArrayElements(outPixels, 0);

  Mat mGray ( height, width, CV_8UC1, (unsigned char *)pNV21FrameData ) ;
  // Mat mGray ( height, width, CV_8UC4, (unsigned char *)pNV21FrameData ) ;
  Mat mResult ( height, width, CV_8UC4, (unsigned char *)poutPixels ) ;
  Mat mImg = imdecode ( mGray, CV_LOAD_IMAGE_COLOR ) ;

  // IplImage srcImg = mGray;
  IplImage srcImg = mImg;
  // IplImage CannyImg = *mCanny;
  IplImage ResultImg = mResult;

  // cvCanny(&srcImg, &CannyImg, 80, 100, 3);
  // cvCvtColor(&CannyImg, &ResultImg, CV_GRAY2BGRA);
  // cvCvtColor ( &srcImg, &ResultImg, CV_GRAY2BGRA ) ;
  // color conversions: http://docs.opencv.org/master/df/d4e/group__imgproc__c.html#gsc.tab=0
  // cvCvtColor ( &srcImg, &ResultImg, CV_RGB2BGRA ) ;
  // Mat mImg = imdecode(mGray, CV_LOAD_IMAGE_COLOR);

  env->ReleaseByteArrayElements(NV21FrameData, pNV21FrameData, 0);
  env->ReleaseIntArrayElements(outPixels, poutPixels, 0);

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
