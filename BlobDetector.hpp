#ifndef BLOB_DETECTOR_H
#define BLOB_DETECTOR_H

#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;


class BlobDetector{
private:
  Ptr<BackgroundSubtractorMOG2> backgroungSubtractor;
  Mat morphologicalKernel;
  vector< vector<Point>  > hulls;
  int minAreaBlob;
  int maxAreaBlob;
  int MIN_THRESHOLD;
  int DEST_THRESHOLD;
  

public:
  BlobDetector(int kernelShape, Size kernelSize, Point kernelAnchorPoint, int minArea = 0, int maxArea = 1000000, int minT=240, int maxT=255);
  vector< vector<Point> > getBlobs();
  void boundBlobs(Mat frame, Scalar color);
  void drawBlobs(Mat frame, Scalar color);
  void morphologyOperations(Mat orig, Mat dest);
  Mat findBlobs(Mat orig);
  void resetBackground();
};

#endif
