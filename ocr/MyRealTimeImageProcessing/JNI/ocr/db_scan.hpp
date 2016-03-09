
/*! \file db_scan.hpp */

#ifndef DB_SCAN_HPP
#define DB_SCAN_HPP

#ifdef __cplusplus

#ifndef SKIP_INCLUDES
// includes here
#include <opencv2/core/core.hpp>
#include <map>

#endif // SKIP_INCLUDES

using namespace cv;
// using namespace std;

class DbScan {
#define DP(i,j) dp[(data.size()*i)+j]
public:
  DbScan ( std::vector<Rect>& _data, double _eps, int _mnpts );
  std::map<int, int> labels;
  std::vector<Rect>& data;
  int C;
  double eps;
  int mnpts;
  double* dp;
  void run ( );
  std::vector<std::vector<Rect> > getGroups ( );
private:
  void expandCluster ( int p,std::vector<int> neighbours );
  bool isVisited(int i);
  std::vector<int> regionQuery ( int p );
  double dist2d(Point2d a,Point2d b);
  double distanceFunc ( int ai, int bi );
};

#endif // __cplusplus
#endif /*DB_SCAN_HPP*/

