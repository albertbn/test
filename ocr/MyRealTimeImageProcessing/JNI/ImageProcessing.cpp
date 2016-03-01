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

extern "C"

jboolean
Java_my_project_MyRealTimeImageProcessing_CameraPreview_ImageProcessing (
		JNIEnv* env, jobject thiz,
		jint width, jint height,
		jbyteArray NV21FrameData, jintArray outPixels)
{
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

// http://stackoverflow.com/questions/22752555/need-an-advice-processing-color-image-from-camera-android-opencv-ndk-c
void convertYUV ( JNIEnv* env, int width, int height, jbyteArray yuvArray, Mat &img ) {

     // Get the data from JEnv.
    signed char *data = env->GetByteArrayElements(yuvArray, 0);

     // Convert to Mat object.
     Mat imgbuf( Size(width,height), CV_8UC4, (unsigned char*) data );
     img = imdecode(imgbuf, CV_LOAD_IMAGE_COLOR);

    // Release the JNI data pointer.
    env->ReleaseByteArrayElements(yuvArray, (jbyte*) yuvArray, 0);
}

jboolean
Java_my_project_MyRealTimeImageProcessing_CameraPreview_colourDetect (
		JNIEnv* env, jobject thiz,
		jint width, jint height,
		jbyteArray NV21FrameData ) {

  // jbyte * pNV21FrameData = env->GetByteArrayElements(NV21FrameData, 0);

  Mat mFrame;
  convertYUV ( env, width, height, NV21FrameData, mFrame );
  // Mat mFrame ( height, width, CV_8UC1, (unsigned char *)pNV21FrameData );
  do_frame ( mFrame );

  // env->ReleaseByteArrayElements(NV21FrameData, pNV21FrameData, 0);

  return true;
}

