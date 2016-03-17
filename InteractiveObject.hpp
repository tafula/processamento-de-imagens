#ifndef INTERACTIVEOBJECT_H
#define INTERACTIVEOBJECT_H

#include<opencv2/opencv.hpp>
#include<unistd.h>
#include<sys/time.h>

using namespace cv;
using namespace std;

class InteractiveObject{

private:
  unsigned int activationTime;
  unsigned int activeStamp;
  unsigned int count;
  bool counted;
  bool active;
  Rect rectangle;

public:
  InteractiveObject(Rect rectangle, unsigned int activationTime = 500);
  bool ProcessHulls(vector< vector<Point> > hulls);
  void Draw(Mat &frame);
  unsigned int GetCount();
  void Deactivate();
  bool IsActive();
  bool IsCounted();

private:
  unsigned int GetTimestamp();
  bool IsInsider(Point referencePoint);
};

#endif
