#include <stdio.h>
#include <iostream>
#include "MKalman.hpp"



MKalman::MKalman(int inix, int iniy): KF(4,2,0)
{

 // Kalman filter transformation matrix
  KF.transitionMatrix = (Mat_<float>(4, 4) << 
  			  1,0,1,0,
  			  0,1,0,1,
  			  0,0,1,0,
  			  0,0,0,1); 
 
  /*Initializes the first measurements*/
  KF.statePre.at<float>(0) = inix;
  KF.statePre.at<float>(1) = iniy;
  KF.statePre.at<float>(2) = 0;
  KF.statePre.at<float>(3) = 0;
  
  setIdentity(KF.measurementMatrix);
  setIdentity(KF.processNoiseCov, Scalar::all(1e-4));
  setIdentity(KF.measurementNoiseCov, Scalar::all(1e-1));
  setIdentity(KF.errorCovPost, Scalar::all(.1));
}


/*Makes new predition based on former data.*/
Point MKalman::predict()
{
  Mat prediction = KF.predict();
  Point predictPt(prediction.at<float>(0),prediction.at<float>(1));
  return predictPt;
}

//Updates information given the actual outcome
void MKalman::correct(Point2f p)
{
  Mat_<float> measurement = (Mat_<float>(2,1) << p.x, p.y);
  KF.correct(measurement);
}




