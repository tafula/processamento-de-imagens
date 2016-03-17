#include "TrackObj.hpp"

using namespace std;

/*Uses the associated Kalman Filter to predict the next position the
 Tracked Object will be at*/
void TrackObj::predict()
{
  if(history.size() == 1) history.push_back(history[0]);
  else history.push_back(filter.predict());
  nFramesUnseen++;
}

/*Updates the filter's datas*/
void TrackObj::correct(Point2f p)
{
  filter.correct(p);
  nFramesUnseen = 0;
}

//returns the number of frames since this object was last detected
int TrackObj::framesUnseen()
{
  return nFramesUnseen;
}

//returns the position the object was last observed at
Point2f TrackObj::lastHistory()
{
  return history[history.size()-1];
}

//draws trajectory of tracked object
void TrackObj::drawTrajectory(Mat dest){
  if(history.size() == 1)
    circle(dest, lastHistory(), 4, color, -1, 8, 0);
  else{
    for(unsigned int i = 1; i < history.size(); i++){
      line(dest, history[i-1], history[i], color, 2, 0);
    }
  }
}


