#include "InteractiveObject.hpp"

InteractiveObject::InteractiveObject(Rect rectangle, unsigned int activationTime) {
  this->rectangle = rectangle;
            
  this->activationTime = activationTime;
  this->activeStamp= 0;
  this->count = 0;
  this->counted = false;
  this->active = false;
}

bool InteractiveObject::ProcessHulls(vector< vector<Point> > hulls) {
  bool foundInsider = false;
  for (auto hull: hulls) {
    for (auto pt: hull) {
      if (IsInsider(pt)) {
	foundInsider = true;
	break;
      }
    }
  }

  bool oneMore = false;
	    
  if(this->active){
    if(!foundInsider)
      Deactivate();
	      
    else if(!this->counted && ((GetTimestamp() - this->activeStamp) > this->activationTime)){
      this->count++;
      this->counted = true;
      oneMore = true;
    }
  }

  else if(foundInsider){
    this->active = true;
    this->activeStamp = GetTimestamp();
  }
            
  return oneMore;
}
    
void InteractiveObject::Draw(Mat &frame) {
  int thickness = 1;
  if (this->active) {
    thickness = 3;
  }
  if (this->counted) {
    thickness = -1;
  }
  cv::rectangle(frame, this->rectangle ,  Scalar(255, 0, 0), thickness);
}

unsigned int InteractiveObject::GetCount() {
  return this->count;
}
    
void InteractiveObject::Deactivate() {
  this->active = false;
  this->activeStamp= 0;
  this->counted = false;
}

bool InteractiveObject::IsActive() {
  return this->active;
}

bool InteractiveObject::IsCounted(){
  return this->counted;
}

unsigned int InteractiveObject::GetTimestamp() {
  unsigned int uitime;
  timeval time;
  gettimeofday(&time, NULL);
  uitime = time.tv_sec * 1000 + time.tv_usec/1000;
  return uitime; 
}

bool InteractiveObject::IsInsider(Point referencePoint) {
  bool retval = false;
  if (referencePoint.x > this->rectangle.x &&
      referencePoint.x < (this->rectangle.x + this->rectangle.width) &&
      referencePoint.y > this->rectangle.y &&
      referencePoint.y < (this->rectangle.y + this->rectangle.height)) {
    retval = true;
  }
  return retval;
}    
