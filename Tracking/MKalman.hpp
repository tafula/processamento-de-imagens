/*
  Authors:
  -Gervasio Protasio dos Santos Neto
  -Victor Sanches Portella
*/

#ifndef MKALMAN_H
#define MKALMAN_H

#include "opencv2/core/core.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;

class MKalman{

  KalmanFilter KF; //The class' Kalman Filter

 public:

  MKalman(int inix, int iniy);
  Point predict();
  void correct(Point2f p);
};

#endif
