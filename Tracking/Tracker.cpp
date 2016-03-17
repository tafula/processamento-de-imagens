#include <stdio.h>
#include <dlib/matrix.h>
#include <iostream>
#include "Tracker.hpp"
#include <dlib/optimization.h>
#include <dlib/optimization/max_cost_assignment.h>

#include <algorithm>

using namespace std;

Scalar setColors[3] = { Scalar(255,0,0),
			Scalar(0,255,0),
			Scalar(0,0,255)};



Tracker::Tracker(){
}

//returns a random scalar, used to create a color to draw a newly observed
//object
static Scalar random_color(){
  int r = rand()%128 + 127;
  int g = rand()%128 + 127;
  int b = rand()%128 + 127;
  return Scalar(b,g,r);
}

//initializes the trackes and adds to it any detected object
void Tracker::init(vector< vector<Point> > blobs)
{
  initialized = true;

  for(unsigned int i = 0; i < blobs.size(); i++){
    Scalar color = random_color();
    Point2f centroid = getBlobCentroid(blobs[i]);
    objects.push_back(TrackObj(color, centroid));
  }
}

Point2f Tracker::getBlobCentroid(vector<Point> blob){
  //uses the objects' moments to calculate their centroid;
  Moments moment = moments(blob, false);
  //obtains the actual centroid
  Point2f centroid = Point2f(moment.m10/moment.m00, moment.m01/moment.m00);
  return centroid;
}

bool Tracker::wasInitialized(){
  return initialized;
}

dlib::matrix<int> Tracker::distance_matrix(vector<Point2f> measurements){
  
  //row i are distances from (all) measurements to the i-th  TrackedObjs
  //column j are the distances from the j-th measument to (all) the TrackObjs
  //distances[i][j] is the distance from the i-th tracked object to the j-th measurement
  unsigned int max;
  unsigned int maxLin = measurements.size();
  unsigned int maxCol = objects.size();
  if(maxLin > maxCol) max = maxLin;
  else max = maxCol;
  dlib::matrix<int> dMatrix(max, max); 
  int smallN = 1;

  //constructs a matrix distance between the predictions made by the Kalman
  //Filters currently in our Tracker and the measurements picked up by the 
  //blob detector.
  for(unsigned int i = 0; i < max; i++){
    for(unsigned int j = 0; j < max; j++){
      int dist;
      if(i >= maxLin || j >= maxCol) dist = smallN++;
      else dist = (int)(-100*norm(measurements[i] - objects[j].history[objects[j].history.size()-1]));
      dMatrix(i,j) = dist;
    }
  }
  return dMatrix;
} 

//Given a distance matrix between Kalman FIlter Predictions and observed 
//measurements, outputs this function will find a way to assign a filter
//to each obejct such as to minimize the total distance. For this, we use an
//implementation of the Hungarian Algorithm, that solves minimum cost
//assignment problems (essentilay fiding a perfect matching of minimum cost
//in a bipartite graph).
void Tracker::track(vector< vector<Point> > blobs)
{
  vector<Point2f> measurements(blobs.size());
  for(unsigned int i = 0; i < measurements.size(); i++){
    measurements[i] = getBlobCentroid(blobs[i]);
  }
  
  //Acquires the Kalman Filter Predictions
  for(unsigned int i = 0; i < objects.size(); i++)
    objects[i].predict();

  //Builds the distance matrix
  dlib::matrix<int> cost_matrix = distance_matrix(measurements);
  vector<long int> assignments;

  //Uses the dlib optimization library's implementation of the Hungarioan 
  //Algorithm to find the optimal pairing
  assignments = dlib::max_cost_assignment(cost_matrix);

  //vector that will keep track of objects that were not paired to any filter
  //prediction. Not being paired with a prediction is an indicator that the
  //object has left hte scene.
  vector<bool> msNotAssigned(measurements.size());
  fill(msNotAssigned.begin(),msNotAssigned.end(), false);

  double tresh = 100;
  //Uses the measurements to correct the predictions of the associated Kalman
  //Filters.
  for(unsigned int i = 0; i < measurements.size(); i++)
    {
      int assig = assignments[i];
      if(assig >= objects.size()) msNotAssigned[i] = true;
      else
	if(norm(objects[assig].lastHistory() - measurements[i]) > tresh)
	  msNotAssigned[i] = true;
	else objects[assig].correct(measurements[i]);
    }

  //Erase lost objects
  objects.erase(remove_if(objects.begin(), objects.end(), objectIsLost), objects.end()); 

  //Create new trackers for non matched measurements
  for(unsigned int i = 0; i < msNotAssigned.size(); i++)
    if(msNotAssigned[i]){
      Scalar color = random_color();
      objects.push_back(TrackObj(color, measurements[i]));
    }
}

void Tracker::drawTrackedTrajectories(Mat dest){
  for(unsigned int i = 0; i < objects.size(); i++)
    objects[i].drawTrajectory(dest);
}
