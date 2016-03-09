
// /**
//  * Possible modes for page layout analysis. These *must* be kept in order
//  * of decreasing amount of layout analysis to be done, except for OSD_ONLY,
//  * so that the inequality test macros below work.
// */
// enum PageSegMode {
//   PSM_OSD_ONLY,       ///< Orientation and script detection only.
//   PSM_AUTO_OSD,       ///< Automatic page segmentation with orientation and
//                       ///< script detection. (OSD)
//   PSM_AUTO_ONLY,      ///< Automatic page segmentation, but no OSD, or OCR.
//   PSM_AUTO,           ///< Fully automatic page segmentation, but no OSD.
//   PSM_SINGLE_COLUMN,  ///< Assume a single column of text of variable sizes.
//   PSM_SINGLE_BLOCK_VERT_TEXT,  ///< Assume a single uniform block of vertically
//                                ///< aligned text.
//   PSM_SINGLE_BLOCK,   ///< Assume a single uniform block of text. (Default.)
//   PSM_SINGLE_LINE,    ///< Treat the image as a single text line.
//   PSM_SINGLE_WORD,    ///< Treat the image as a single word.
//   PSM_CIRCLE_WORD,    ///< Treat the image as a single word in a circle.
//   PSM_SINGLE_CHAR,    ///< Treat the image as a single character.
//   PSM_SPARSE_TEXT,    ///< Find as much text as possible in no particular order.
//   PSM_SPARSE_TEXT_OSD,  ///< Sparse text with orientation and script det.
//   PSM_RAW_LINE,       ///< Treat the image as a single text line, bypassing
//                       ///< hacks that are Tesseract-specific.

//   PSM_COUNT           ///< Number of enum entries.
// };

#include <opencv2/opencv.hpp>
#include "db_scan.hpp"
#include "tess.hpp"
#include "main.hpp"
// #include "../static_fields.hpp"
// #include "../common.hpp"

// int threshold_value = 60;
// int threshold_type = THRESH_BINARY;
// int const max_BINARY_value = 255;

int px_line_height = 90; /* TODO - dynamic */
int px_expand_bound_line = 90; /* TODO - dynamic */
int px_trim_sides = 0; /* TODO - figure out how much to trim sides */

// using namespace cv;
struct less_custom_sort_points {

    inline bool operator() ( const Rect& struct1, const Rect& struct2 ) {
      return ( struct1.tl().y <  struct2.tl().y );
    }
};

template <class T>
inline string to_string ( const T& t ) {

  std::stringstream ss;
  ss << t;
  return ss.str();
}

std::vector<cv::Rect> detectLetters ( cv::Mat img ) {

    std::vector<cv::Rect> boundRect;
    cv::Mat img_gray, img_sobel, img_threshold, element;
    img_gray = img;

    cv::Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);

    cv::threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);

    element = getStructuringElement(cv::MORPH_RECT, cv::Size(17, 3) );
    cv::morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element); //Does the trick

    std::vector< std::vector< cv::Point> > contours;
    cv::findContours(img_threshold, contours, 0, 1);
    // std::cout << "contours: " << contours.size() << std::endl;

    std::vector<std::vector<cv::Point> > contours_poly( contours.size() );

    for ( int i = 0; i < (int)contours.size(); i++ ) {
      if (contours[i].size()>50)
        {
          cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
          cv::Rect appRect( boundingRect( cv::Mat(contours_poly[i]) ));
          if(appRect.height<200){

            boundRect.push_back(appRect);
          }
        }
    }

    return boundRect;
}

cv::Scalar HSVtoRGBcvScalar ( int H, int S, int V ) {

    int bH = H; // H component
    int bS = S; // S component
    int bV = V; // V component
    double fH, fS, fV;
    double fR, fG, fB;
    const double double_TO_BYTE = 255.0f;
    const double BYTE_TO_double = 1.0f / double_TO_BYTE;

    // Convert from 8-bit integers to doubles
    fH = (double)bH * BYTE_TO_double;
    fS = (double)bS * BYTE_TO_double;
    fV = (double)bV * BYTE_TO_double;

    // Convert from HSV to RGB, using double ranges 0.0 to 1.0
    int iI;
    double fI, fF, p, q, t;

    if( bS == 0 ) {
        // achromatic (grey)
        fR = fG = fB = fV;
    }
    else {
        // If Hue == 1.0, then wrap it around the circle to 0.0
        if (fH>= 1.0f)
            fH = 0.0f;

        fH *= 6.0; // sector 0 to 5
        fI = floor( fH ); // integer part of h (0,1,2,3,4,5 or 6)
        iI = (int) fH; // " " " "
        fF = fH - fI; // factorial part of h (0 to 1)

        p = fV * ( 1.0f - fS );
        q = fV * ( 1.0f - fS * fF );
        t = fV * ( 1.0f - fS * ( 1.0f - fF ) );

        switch( iI ) {
        case 0:
            fR = fV;
            fG = t;
            fB = p;
            break;
        case 1:
            fR = q;
            fG = fV;
            fB = p;
            break;
        case 2:
            fR = p;
            fG = fV;
            fB = t;
            break;
        case 3:
            fR = p;
            fG = q;
            fB = fV;
            break;
        case 4:
            fR = t;
            fG = p;
            fB = fV;
            break;
        default: // case 5 (or 6):
            fR = fV;
            fG = p;
            fB = q;
            break;
        }
    }

    // Convert from doubles to 8-bit integers
    int bR = (int)(fR * double_TO_BYTE);
    int bG = (int)(fG * double_TO_BYTE);
    int bB = (int)(fB * double_TO_BYTE);

    // Clip the values to make sure it fits within the 8bits.
    if (bR > 255)
        bR = 255;
    if (bR < 0)
        bR = 0;
    if (bG >255)
        bG = 255;
    if (bG < 0)
        bG = 0;
    if (bB > 255)
        bB = 255;
    if (bB < 0)
        bB = 0;

    // Set the RGB cvScalar with G B R, you can use this values as you want too..
    return cv::Scalar(bB,bG,bR); // R component
}

void ocr_doit ( Mat& im_orig, ofstream outfile_ocr ) {

  // init tess
  init_ocr();

  // tess.SetImage ( (uchar*)im_orig.data, im_orig.cols, im_orig.rows, 1, im_orig.cols );
  // orientation_check(im_orig);
  if ( im_orig.cols>im_orig.rows )
    rot90(im_orig,1);


  Mat im = im_orig.clone();
  Mat grouped = Mat::zeros(im.size(),CV_8UC3);
  std::vector<Rect> boxes = detectLetters(im);

  // for ( int i=0; i<(int)boxes.size(); ++i ) {
  //   cv::rectangle(im,boxes[i],cv::Scalar(0,255,0),3,8,0);
  // }

  DbScan dbscan ( boxes, px_line_height, 2 ); /*HERE - expected tuning or calculating of the eps param*/
  dbscan.run();

  std::vector<Scalar> colors;
  RNG rng(3);
  for ( int i=0;i<=dbscan.C;i++ ) {
    colors.push_back(HSVtoRGBcvScalar(rng(255),255,255));
  }

  for ( int i=0;i<(int)dbscan.data.size();i++ ) {
    Scalar color;
    if(dbscan.labels[i]==-1) {
      color=Scalar(128,128,128);
    }
    else {
      int label=dbscan.labels[i];
      color=colors[label];
    }

    putText ( grouped, to_string(dbscan.labels[i]),dbscan.data[i].tl(), FONT_HERSHEY_COMPLEX,.5,color,1);
  }

  std::vector<std::vector<Rect> > ggroups = dbscan.getGroups(); /*get the grouped rects*/

  Rect r0;
  std::vector<Rect> rect_lines;
  std::vector<cv::Point> points0;
  Point tl, br;
  tess.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
  // tess.SetPageSegMode ( tesseract::PSM_SINGLE_BLOCK ); /*see top of page for PSM_ enum*/
  // tess.SetPageSegMode ( tesseract::PSM_SINGLE_LINE ); /*see top of page for PSM_ enum*/
  // tess.SetPageSegMode ( tesseract::PSM_RAW_LINE ); /*see top of page for PSM_ enum*/
  // tess.SetPageSegMode(tesseract::PSM_AUTO);

  for ( int i=0; i<(int)ggroups.size(); ++i ) {

    points0.clear();
    for(int j=0; j<(int)ggroups[i].size(); ++j){
      points0.push_back(ggroups[i][j].tl());
      points0.push_back(ggroups[i][j].br());
    }
    r0 = cv::boundingRect(points0);
    tl = r0.tl(); br = r0.br();
    tl.x=px_trim_sides; br.x=grouped.cols-px_trim_sides;
    tl.y-(px_expand_bound_line)>=0 && (tl.y-=(px_expand_bound_line));
    // br.y-(px_expand_bound_line)>=0 && (br.y-=(px_expand_bound_line));
    br.y+px_expand_bound_line<=grouped.rows && (br.y+=px_expand_bound_line);
    r0 = Rect ( tl, br );
    cv::rectangle ( grouped, r0, colors[i], 3, 8, 0 );
    rect_lines.push_back(r0);
  }

  // HERE - sort and tess each line
  std::sort ( rect_lines.begin(), rect_lines.end(), less_custom_sort_points() );

  for ( int i=0; i<(int)rect_lines.size(); ++i ) {
    // std::cout << "rect:\t" << rect_lines[i] << std::endl;
    if(7==7) crop_b_tess ( im_orig, rect_lines[i], i, outfile_ocr );
    // if(7==7 && rect_lines[i].height<1120) crop_b_tess ( im_orig, rect_lines[i] );
  }

  // for ( int i=0; i<(int)boxes.size(); ++i ) {
  //   cv::rectangle(grouped,boxes[i],cv::Scalar(255,255,255),3,8,0);
  // }
}