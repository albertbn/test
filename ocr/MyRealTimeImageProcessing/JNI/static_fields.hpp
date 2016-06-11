
/*! \file static_fields.hpp */

#ifndef STATIC_FIELDS
#define STATIC_FIELDS

#ifdef __cplusplus

#ifndef SKIP_INCLUDES
// includes here
#include <opencv2/core/core.hpp>
#endif // SKIP_INCLUDES

using namespace cv;
using namespace std;

extern ofstream outfile;
extern ofstream outfile_ocr;
extern string path_sd_card;
extern string IMG_PATH;

extern vector < vector<Point> > contours_poly2;

#endif // __cplusplus
#endif /*STATIC_FIELDS*/

