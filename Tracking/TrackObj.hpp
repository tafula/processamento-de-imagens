/*Authors:
  -Gervasio Protasio dos Santos Neto
  -Victor Sanches Portella
*/

#ifndef TRACKOBJ_H
#define TRACKOBJ_H

#include "opencv2/core/core.hpp"
#include "MKalman.hpp"

using namespace std;
using namespace cv;

class TrackObj{
 
 public:
  vector<Point2f> history; //Point the object has been in the previous frames
  Scalar color; //color in which the objectcs trajectories will be drawn
  void drawTrajectory(Mat dest);


 TrackObj(Scalar colorA, Point2f iniP):
  color(colorA),
    filter(iniP.x, iniP.y)
      {nFramesUnseen = 0;
	history.push_back(iniP);}
  
  void predict();
  void correct(Point2f p);
  int framesUnseen();
  Point2f lastHistory();

 private:
  MKalman filter; //Each tracked object is associated with a Kalman FIlter
  int nFramesUnseen; //For how many frames the object hasnt been detected

};


#endif
