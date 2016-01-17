
#include <opencv2/opencv.hpp>
#include <map>
#include <sstream>
#include "db_scan.hpp"

using namespace cv;

DbScan::DbScan ( std::vector<Rect>& _data, double _eps, int _mnpts ) : data ( _data ){
  C=-1;
  for ( int i=0;i<(int)data.size();i++ ) {
    labels[i]=-99;
  }
  eps=_eps;
  mnpts=_mnpts;
}

void DbScan :: run(){

}

std::vector<std::vector<Rect> > DbScan :: getGroups ( ){

  std::vector<std::vector<Rect> > ret;

  return ret;
}
