
// g++ $(pkg-config --cflags --libs opencv) lines_dbscan.cc -o lines_dbscan && ./lines_dbscan

#include "opencv2/opencv.hpp"
#include <map>
#include <sstream>

using namespace cv;

template <class T>
inline std::string to_string (const T& t) {

  std::stringstream ss;
  ss << t;
  return ss.str();
}

class DbScan {
public:
  std::map<int, int> labels;
  std::vector<Rect>& data;
  int C;
  double eps;
  int mnpts;
  double* dp;

  //memoization table in case of complex dist functions
#define DP(i,j) dp[(data.size()*i)+j]
  DbScan ( std::vector<Rect>& _data, double _eps, int _mnpts ) : data ( _data ) {
    C=-1;
    for ( int i=0;i<(int)data.size();i++ ) {
      labels[i]=-99;
    }
    eps=_eps;
    mnpts=_mnpts;
  }
  void run() {
    dp = new double[data.size()*data.size()];
    for ( int i=0;i<(int)data.size();i++ ) {
      for ( int j=0;j<(int)data.size();j++ ) {
        if(i==j)
          DP(i,j)=0;
        else
          DP(i,j)=-1;
      }
    }
    for(int i=0;i<(int)data.size();i++) {
      if(!isVisited(i)) {
        std::vector<int> neighbours = regionQuery(i);
        if((int)neighbours.size()<mnpts) {
          labels[i]=-1;//noise
        }
        else {
          C++;
          expandCluster(i,neighbours);
        }
      }
    }
    delete [] dp;
  }
  void expandCluster(int p,std::vector<int> neighbours) {
    labels[p]=C;
    for(int i=0;i<(int)neighbours.size();i++) {
      if(!isVisited(neighbours[i])) {
        labels[neighbours[i]]=C;
        std::vector<int> neighbours_p = regionQuery(neighbours[i]);
        if ((int)neighbours_p.size() >= mnpts) {
          expandCluster(neighbours[i],neighbours_p);
        }
      }
    }
  }

  bool isVisited(int i) {
    return labels[i]!=-99;
  }

  std::vector<int> regionQuery(int p) {
    std::vector<int> res;
    for(int i=0;i<(int)data.size();i++) {
      if(distanceFunc(p,i)<=eps) {
        res.push_back(i);
      }
    }
    return res;
  }

  double dist2d(Point2d a,Point2d b) {
    // return sqrt(pow(a.x-b.x,2) + pow(a.y-b.y,2));
    return abs(a.x-b.x); /*TODO - by vertical or horizontal*/
  }

  double distanceFunc(int ai,int bi) {

    if(DP(ai,bi)!=-1)
      return DP(ai,bi);

    Rect a = data[ai];
    Rect b = data[bi];

    // Point2d cena= Point2d(a.x+a.width/2, a.y+a.height/2);
    // Point2d cenb = Point2d(b.x+b.width/2, b.y+b.height/2);
    // double minDist = abs(data[ai].x-data[bi].x); /*Point2d*/
    // double minDist = abs(data[ai].x-data[bi].x); /*TODO - by vertical or horizontal*/
    // DP(ai,bi)=minDist;
    // DP(bi,ai)=minDist;
    // return DP(ai,bi);

    // Point2d cena= Point2d(a.x+a.width/2,
    // a.y+a.height/2);
    // double dist = sqrt(pow(cena.x-cenb.x,2) + pow(cena.y-cenb.y,2));
    // DP(ai,bi)=dist;
    // DP(bi,ai)=dist;

    Point2d tla =Point2d(a.x,a.y);
    Point2d tra =Point2d(a.x+a.width,a.y);
    Point2d bla =Point2d(a.x,a.y+a.height);
    Point2d bra =Point2d(a.x+a.width,a.y+a.height);

    Point2d tlb =Point2d(b.x,b.y);
    Point2d trb =Point2d(b.x+b.width,b.y);
    Point2d blb =Point2d(b.x,b.y+b.height);
    Point2d brb =Point2d(b.x+b.width,b.y+b.height);

    double minDist = 9999999;

    minDist = min(minDist,dist2d(tla,tlb));
    minDist = min(minDist,dist2d(tla,trb));
    minDist = min(minDist,dist2d(tla,blb));
    minDist = min(minDist,dist2d(tla,brb));

    minDist = min(minDist,dist2d(tra,tlb));
    minDist = min(minDist,dist2d(tra,trb));
    minDist = min(minDist,dist2d(tra,blb));
    minDist = min(minDist,dist2d(tra,brb));

    minDist = min(minDist,dist2d(bla,tlb));
    minDist = min(minDist,dist2d(bla,trb));
    minDist = min(minDist,dist2d(bla,blb));
    minDist = min(minDist,dist2d(bla,brb));

    minDist = min(minDist,dist2d(bra,tlb));
    minDist = min(minDist,dist2d(bra,trb));
    minDist = min(minDist,dist2d(bra,blb));
    minDist = min(minDist,dist2d(bra,brb));

    DP(ai,bi)=minDist;
    DP(bi,ai)=minDist;
    return DP(ai,bi);
  }

  std::vector<std::vector<Rect> > getGroups() {
    std::vector<std::vector<Rect> > ret;
    for(int i=0;i<=C;i++) {
      ret.push_back(std::vector<Rect>());
      for(int j=0;j<(int)data.size();j++) {
        if(labels[j]==i) {
          ret[ret.size()-1].push_back(data[j]);
        }
      }
    }
    return ret;
  }
};

cv::Scalar HSVtoRGBcvScalar(int H, int S, int V) {

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

std::vector<cv::Rect> detectLetters ( cv::Mat img ) {

    std::vector<cv::Rect> boundRect;
    cv::Mat img_gray, img_sobel, img_threshold, element;
    // cv::Mat img_sobel, img_threshold, element;
    // cvtColor(img, img_gray, CV_BGR2GRAY);
    // cvtColor(img, img_gray, CV_RGBA2GRAY);
    img_gray = img;

    cv::Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    // cv::Sobel(img_gray, img_sobel, CV_8UC3, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);

    cv::threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);
    // img_threshold = img_sobel;

    element = getStructuringElement(cv::MORPH_RECT, cv::Size(17, 3) );
    cv::morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element); //Does the trick

    cv::imwrite ( "./img_pre/lines_dbscan1.jpg", img_threshold ) ;

    std::vector< std::vector< cv::Point> > contours;
    cv::findContours(img_threshold, contours, 0, 1);
    std::cout << "contours: " << contours.size() << std::endl;

    std::vector<std::vector<cv::Point> > contours_poly( contours.size() );

    for ( int i = 0; i < (int)contours.size(); i++ ) {
      if (contours[i].size()>50)
        {
          cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
          cv::Rect appRect( boundingRect( cv::Mat(contours_poly[i]) ));
          boundRect.push_back(appRect);
        }
    }

    return boundRect;
}

int main ( int argc, char** argv ) {

  // Mat im = imread( "./img_pre/long8.jpg");
  Mat im = imread( "./img_pre/long8.jpg", 0);
  Mat grouped = Mat::zeros(im.size(),CV_8UC3);

  cv::imwrite ( "./img_pre/lines_dbscan00.jpg", im ) ;

  std::vector<Rect> boxes = detectLetters(im);

  // std::vector<Point2d> rect_points;
  // Rect a;
  for ( int i=0; i<(int)boxes.size(); ++i ) {
    // a = boxes[i];
    // Point2d cena= Point2d(a.x+a.width/2, a.y+a.height/2);
    // // TODO - x or y according to orientation
    // rect_points.push_back(cena);
    cv::rectangle(im,boxes[i],cv::Scalar(0,255,0),3,8,0);
  }
  cv::imwrite ( "./img_pre/lines_dbscan02.jpg", im ) ;

  DbScan dbscan ( boxes, 10, 2 );
  dbscan.run();
  //done, perform display, check emacs git

  std::cout << "dbscan.C: " << dbscan.C << "\ndbscan.data.size(): " << dbscan.data.size() << std::endl;

  std::vector<Scalar> colors;
  RNG rng(3);
  for(int i=0;i<=dbscan.C;i++) {
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

    putText(grouped,to_string(dbscan.labels[i]),dbscan.data[i].tl(), FONT_HERSHEY_COMPLEX,.5,color,1);
    // putText(grouped,to_string(dbscan.labels[i]),dbscan.data[i], FONT_HERSHEY_COMPLEX,.5,color,1);
    // drawContours(grouped,contours,i,color,-1);
  }

  std::vector<std::vector<Rect> > ggroups = dbscan.getGroups();

  Rect r0;
  std::vector<cv::Point> points0;
  for(int i=0; i<(int)ggroups.size(); ++i){
    points0.clear();
    for(int j=0; j<(int)ggroups[i].size(); ++j){
      points0.push_back(ggroups[i][j].tl());
      points0.push_back(ggroups[i][j].br());
    }
    r0 = cv::boundingRect(points0);
    r0 = Rect(Point(r0.tl().x, 0), Point(r0.br().x, grouped.cols)); /*TODO - hor or vertical - use x for hor lying image, else y, also cols or rows*/
    cv::rectangle ( grouped, r0, colors[i], 3, 8, 0 );

    // std::cout << "dbscan.groups: " << Mat(ggroups[i]) << std::endl;
  }

  cv::imwrite ( "./img_pre/lines_dbscan03.jpg", grouped ) ;

  return 0;
}
