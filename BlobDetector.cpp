#include "BlobDetector.hpp"

BlobDetector::BlobDetector(int kernelShape, Size kernelSize, Point kernelAnchorPoint, int minArea, int maxArea, int minT, int maxT){
  morphologicalKernel = getStructuringElement(kernelShape, kernelSize, kernelAnchorPoint);
  minAreaBlob = minArea;
  maxAreaBlob = maxArea;
  backgroungSubtractor = createBackgroundSubtractorMOG2();
  MIN_THRESHOLD = minT;
  DEST_THRESHOLD = maxT;
}

void BlobDetector::resetBackground(){
  backgroungSubtractor = createBackgroundSubtractorMOG2();
}

void BlobDetector::morphologyOperations(Mat orig, Mat dest){
  //blur(orig, dest,Size(3,3));
  blur(orig, dest, Size(5,5));

  dilate(orig, dest, morphologicalKernel);
  erode(orig, dest, morphologicalKernel);

  erode(orig, dest, morphologicalKernel);
  dilate(orig, dest, morphologicalKernel);
  
  dilate(orig,dest,morphologicalKernel);
}

Mat BlobDetector::findBlobs(Mat orig){
  Mat foreground;
  vector < vector<Point> > contours;

  backgroungSubtractor->apply(orig.clone(), foreground, 0.0001); //DESCOBRIR O QUE E ESSE TERCEIRO PARAMETRO

  threshold(foreground, foreground, MIN_THRESHOLD, DEST_THRESHOLD, CV_THRESH_BINARY);

  findContours(foreground.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

  hulls.clear();
  
  for(int i = 0; i < contours.size(); i++){
    vector<Point> hull;
    convexHull( Mat(contours[i]), hull, false);
    double area = contourArea(hull);
    if(area > minAreaBlob && area < maxAreaBlob)
      hulls.push_back(hull);
  }

  return foreground;
}

vector< vector<Point> > BlobDetector::getBlobs(){
  return hulls;
}

void BlobDetector::drawBlobs(Mat frame, Scalar color){
  for(int i = 0; i < hulls.size(); i++){
    drawContours(frame, hulls, i, color, 1, 8, vector<Vec4i>(), 0, Point());
  }
}

void BlobDetector::boundBlobs(Mat frame, Scalar color){
  vector< vector<Point> >::const_iterator it = hulls.begin();
  vector< vector<Point> >::const_iterator end = hulls.end();
  
  while(it != end){
    Rect bounds = boundingRect(*it);
    rectangle(frame, bounds, color, 2);
    ++it;
  }
}










