
// from jni
// g++ -I"$JAVA_HOME/include" -I"$JAVA_HOME/include/darwin" -shared  -o preNocr.so diordve_bonebou_preNocr.cc -llept -ltesseract

// from src
// javac diordve/bonebou/preNocr.java && java diordve/bonebou/preNocr

// g++ -o preNocr preNocr.cc -llept -ltesseract && ./preNocr

// javac preNocr.java && java preNocr

// tess source
// https://fossies.org/dox/tesseract-ocr-3.02.02/baseapi_8cpp_source.html#l00213
// http://stackoverflow.com/questions/2483978/best-way-to-implement-globally-scoped-data - c++ extern and static
// http://p2p.wrox.com/c-programming/92954-c-class-example-separate-header-implementation-file.html - c++ class header and implement

// TEMP
#include <jni.h>

#ifndef ANDROID
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "util/signalHandler.hpp"
#endif //ANDROID

#include <iostream>
#include <fstream>
#include <ctime>

#include <sys/types.h>
#include <sys/stat.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

//====bonebou_includes===========
#include "diordve_bonebou_preNocr.h"
#include "all_headers.hpp"
#include "util/main_geometry.hpp"
//====end_bonebou_includes=======

using namespace cv;
using namespace std;

unsigned int clock_start;
ofstream outfile;
ofstream outfile_ocr;
std::vector<cv::Vec4i> lines4intersect;
std::vector<bool> lines4intersect_is_vert;
std::vector<Point> p_from_line_vector;
cv::Point center(0,0);
Size size_mat;
double MIN_LINE_LENGTH_CONSIDERED_SIDE;
string path_sd_card; /* doesn't end with / */
string path_img; /* doesn't end with / */

// go on from here
// http://answers.opencv.org/question/14881/hidden-symbol-__aeabi_atexit-in-is-referenced-by-dso/

//TO FORMAT FOR GDOCS
// sed -i '' "s/: /$(printf '\t')/g" dump.txt

#ifdef ANDROID
JNIEXPORT void JNICALL Java_diordve_bonebou_preNocr_doit (
    JNIEnv *env, jobject thisObj, jstring jpath_sd_card, jstring jimg_path ) {

  unsigned int clock_start_main = clock();
  clock_start = clock();

  path_sd_card = (*env).GetStringUTFChars(jpath_sd_card, 0);
  string path_dump = path_sd_card + "/tessdata/img"; /*here will be saved just dump.txt (performance benchmark) and dump_ocr.txt (ocr text result)*/
  path_img = path_sd_card + "/tessdata/img/scrap"; /*this is a global extern var, used by other partial folks */

  if ( !directory_exists( path_dump )) mkdir(path_dump.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); /*create ./img dir if not there*/
  if ( !directory_exists( path_img )) mkdir(path_img.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);  /*create ./img/scrap dir if not there*/
  if ( !directory_exists( path_img+"/dbscan" )) mkdir( (path_img+"/dbscan").c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);  /*create ./img/scrap/dbscan dir if not there*/

  system(("exec rm -r " + path_img +"/*.jpg" ).c_str());/*clear content of ./img/scrap*/
  system(("exec rm -r " + path_img +"/dbscan/*.jpg" ).c_str());/*clear content of ./img/scrap*/
  remove ( (path_sd_card + "/tessdata/dump.txt").c_str() );
  remove ( (path_sd_card + "/tessdata/dump_ocr.txt").c_str() );
  outfile.open ( (path_sd_card + "/tessdata/dump.txt").c_str(), ios_base::app );
  outfile_ocr.open ( (path_sd_card + "/tessdata/dump_ocr.txt").c_str(), ios_base::app );


  // TEMP test
  Mat mat = imread ( path_sd_card + "/tessdata/heb.jpg" ); /*yep!*/

  // string img_path = (*env).GetStringUTFChars(jimg_path, 0);
  // LOGD ( "processing image path: %s \n", img_path.c_str() );
  // outfile << "starting main (after opening outfile stream): " << clock_ticks_to_ms(clock() - clock_start) << endl; clock_start = clock();
  // Mat mat = imread ( img_path ); /*yep!*/

  longest_closed ( mat /*referral variable */ );

  mat.release();

  outfile << "total time: " << clock_ticks_to_ms(clock() - clock_start_main) << endl;
  outfile_ocr.close();
  outfile.close();
  // (*env).ReleaseStringUTFChars(jpath_sd_card, path_sd_card.c_str());
}
#endif // ANDROID

#ifndef ANDROID
// credits: http://stackoverflow.com/questions/77005/how-to-generate-a-stacktrace-when-my-gcc-c-app-crashes
void handler ( int sig ) {

  void *array[50];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 50);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd ( array, size, STDERR_FILENO );
  free ( array );
  exit(1);
}


int main ( int argc, char** argv ) {

  // signal(SIGINT, handler); // install our handler - for stack trace on error
  // signal(SIGABRT, handler); // install our handler - for stack trace on error
  // signal(SIGFPE, handler); // install our handler - for stack trace on error
  // signal(SIGILL, handler); // install our handler - for stack trace on error
  // signal(SIGSEGV, handler); // install our handler - for stack trace on error
  // signal(SIGTERM, handler); // install our handler - for stack trace on error
  // signal(SIGHUP, handler); // install our handler - for stack trace on error

  int iret;

  try {
    if ( argc < 2  ) {
      cout << "please pass an image" << endl;
      return 1;
    }

    // SignalHandler signalHandler;
    // Register signal handler to handle kill signal
    // signalHandler.setupSignalHandlers();

    unsigned int clock_start_main = clock();
    clock_start = clock();

    cout << "evdroid processing img.. " << argv[1] << endl;

    path_sd_card.clear(); /*this is used just by the Andrew version*/
    string path_dump = "./img"; /*here will be saved just dump.txt (performance benchmark) and dump_ocr.txt (ocr text result)*/
    path_img = "./img/scrap"; /*this is a global extern var, used by other partial folks */

    if ( !directory_exists( path_dump )) mkdir(path_dump.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); /*create ./img dir if not there*/
    if ( !directory_exists( path_img )) mkdir(path_img.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);  /*create ./img/scrap dir if not there*/
    if ( !directory_exists( path_img+"/dbscan" )) mkdir( (path_img+"/dbscan").c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);  /*create ./img/scrap/dbscan dir if not there*/

    system(("exec rm -r " + path_img +"/*.jpg" ).c_str());/*clear content of ./img/scrap*/
    system(("exec rm -r " + path_img +"/dbscan/*.jpg" ).c_str());/*clear content of ./img/scrap*/
    remove( (path_dump + "/dump.txt").c_str() );
    remove( (path_dump + "/dump_ocr.txt").c_str() );

    // return 0;

    outfile.open ( (path_dump + "/dump.txt").c_str(), ios_base::app ); /*regular exe computer*/
    outfile_ocr.open ( (path_dump + "/dump_ocr.txt").c_str(), ios_base::app ); /*regular exe computer*/

    outfile << "starting main (after opening outfile stream): " << clock_ticks_to_ms(clock() - clock_start) << endl; clock_start = clock();
    Mat mat = imread ( argv[1] ); /*yep!*/
    mat_downscale_check ( mat ); /*ref var*/
    longest_closed ( mat /*referral variable */ );

    mat.release();

    outfile << "total time: " << clock_ticks_to_ms(clock() - clock_start_main) << endl;
    outfile_ocr.close();
    outfile.close();

    iret = EXIT_SUCCESS;
  }
  catch ( SignalException& e ) {

    std::cerr << "SignalException: " << e.what() << std::endl;
    iret = EXIT_FAILURE;
  }

  return iret;
}
#endif // NOT ANDREW
