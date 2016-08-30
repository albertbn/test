
/*! \file static_fields.hpp */

#ifndef STATIC_FIELDS
#define STATIC_FIELDS

#ifdef __cplusplus

#ifndef SKIP_INCLUDES
// includes here
#include <jni.h>
#include <opencv2/core/core.hpp>
#include "ofstream_child.hpp"
#endif // SKIP_INCLUDES

using namespace cv;
using namespace std;

extern JNIEnv* envJNI;
extern jobject jobjJNI;

extern bool isOCRprocessing;
extern int fd[2];

// extern ofstream outfile;
extern ofstream_child outfile;
// extern ofstream outfile_ocr;
extern ofstream_child outfile_ocr;

extern string path_sd_card;
extern string IMG_PATH;

extern vector < vector<Point> > contours_poly2;

#endif // __cplusplus
#endif /*STATIC_FIELDS*/
