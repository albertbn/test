
#include <opencv2/opencv.hpp>
#include <map>
#include <sstream>
#include "db_scan.hpp"

using namespace cv;

DbScan::DbScan ( std::vector<Rect>& _data, double _eps, int _mnpts ) : data ( _data ) {
  C=-1;
  for ( int i=0;i<(int)data.size();i++ ) {
    labels[i]=-99;
  }
  eps=_eps;
  mnpts=_mnpts;
}

void DbScan::run ( ) {
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

void DbScan::expandCluster ( int p,std::vector<int> neighbours ) {
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

bool DbScan::isVisited(int i) {
  return labels[i]!=-99;
}

std::vector<int>  DbScan::regionQuery ( int p ) {
  std::vector<int> res;
  for(int i=0;i<(int)data.size();i++) {
    if(distanceFunc(p,i)<=eps) {
      res.push_back(i);
    }
  }
  return res;
}

double DbScan::dist2d(Point2d a,Point2d b) {
  return abs ( a.y-b.y ) ;
}

double DbScan::distanceFunc ( int ai, int bi ) {

  if(DP(ai,bi)!=-1)
    return DP(ai,bi);

  Rect a = data[ai];
  Rect b = data[bi];

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

std::vector<std::vector<Rect> > DbScan::getGroups() {
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
