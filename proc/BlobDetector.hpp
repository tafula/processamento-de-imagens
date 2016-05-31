#ifndef BLOB_DETECTOR_H
#define BLOB_DETECTOR_H

#define vOpenCV 1   /* 0 = OpenCV2; 1 = OpenCV3 */
#define ECC_ELL 1.5

#define learnRate 0.00001 /* Frequencia de atualizacao do BackgroundSubtractor(MOG2) */

#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;


class BlobDetector{
private:
#if vOpenCV
	Ptr<BackgroundSubtractorMOG2> backgroungSubtractor;
#else
	BackgroundSubtractorMOG2 backgroungSubtractor;
#endif
  Mat morphologicalKernel;
  vector< vector<Point> > hulls;
  vector< RotatedRect> minEllipse;
  vector< Vec3f > houghBlobs;
  int minAreaBlob;
  int maxAreaBlob;
  int MIN_THRESHOLD;
  int DEST_THRESHOLD;
  

public:
  BlobDetector(int kernelShape, Size kernelSize, Point kernelAnchorPoint, int minArea = 0, int maxArea = 1000000, int minT=240, int maxT=255);
  vector< vector<Point> > getBlobs();
  vector< RotatedRect > getEllipses();
  vector< Vec3f > getHough();
  void boundBlobs(Mat frame, Scalar color);
  void drawBlobs(Mat frame, Scalar color);
  void drawEllipses(Mat frame, Scalar color);
	void drawHough(Mat frame, Scalar color);
  void morphologyOperations(Mat orig, Mat dest);
  Mat findBlobs(Mat orig);
  Mat findEllipses(Mat orig);
  Mat findHough(Mat orig);
  void resetBackground();
};

#endif
