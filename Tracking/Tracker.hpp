/*Authors:
  -Gervasio Protasio dos Santos Neto
  -Victor Sanches Portella
*/

#ifndef TRACKER_H
#define TRACKER_H


#include "MKalman.hpp"
#include "TrackObj.hpp"
#include <dlib/matrix.h>

using namespace cv;

class Tracker{
  
  bool initialized = false;

 public:
  vector<TrackObj> objects;
  Tracker();
  void track(vector< vector<Point> > blobs);
  void init(vector< vector<Point> > blobs);
  bool wasInitialized();
  void drawTrackedTrajectories(Mat dest);

 private:
  Point2f getBlobCentroid(vector<Point>);
  dlib::matrix<int> distance_matrix(vector<Point2f> measurements);
  static bool objectIsLost(TrackObj obj)
    {  return (obj.framesUnseen() > 3);}
};


#endif
