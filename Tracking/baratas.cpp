#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/video/background_segm.hpp"

#include "MKalman.hpp"
#include "TrackObj.hpp"
#include "Tracker.hpp"
#include "../proc/Util.hpp"
#include "../proc/macVideoDev.h"
#include "../proc/BlobDetector.hpp"

#define EROSION_DILATION_SIZE 3

using namespace cv;
using namespace std;

int main(int argc, char **argv){

  Mat frame, original;
  Tracker tracker;

  int minArea;
  cin >> minArea;
  int maxArea;
  cin >> maxArea;

  time_t camAdaptationStartTime = time(NULL);
  bool camAdapted = false;
    
  BlobDetector detector(cv::MORPH_RECT, cv::Size(2 * EROSION_DILATION_SIZE + 1, 2 * EROSION_DILATION_SIZE + 1), cv::Point(EROSION_DILATION_SIZE, EROSION_DILATION_SIZE), minArea, maxArea);

  cvNamedWindow("Tracking", CV_WINDOW_NORMAL);
  cvSetWindowProperty("Tracking", CV_WND_PROP_AUTOSIZE, CV_WINDOW_AUTOSIZE);
  ScreenSize ss = GetScreenSize();
  cvResizeWindow("Tracking", ss.width, ss.height);


  EnableCameraAutoAdjust(GetVideoNum(argc, argv));
  cv::VideoCapture cap(GetVideoNum(argc, argv));

  while(true){
    cap >> frame;
    frame.copyTo(original);

    if(camAdapted){
      /*Prepares the image*/
      detector.morphologyOperations(frame, frame);

      Mat foreground = detector.findBlobs(frame);

      imshow("Treated Image", foreground);

      detector.boundBlobs(original, Scalar(0, 255, 0));

      vector< vector<Point> > objects = detector.getBlobs();
      
      if(!tracker.wasInitialized()){
	tracker.init(objects);
	continue;
      }

      tracker.track(objects);

      tracker.drawTrackedTrajectories(original);
      
    }
    
    else{
      if ((time(NULL) - camAdaptationStartTime) > ADAPTATION_TIME_SEC) {
	camAdapted = true;
	detector.resetBackground();
	DisableCameraAutoAdjust(GetVideoNum(argc, argv));
      }
      else {
	std::string text = "Configurando...";
	cv::Size txtSz = cv::getTextSize(text, cv::FONT_HERSHEY_PLAIN, 4, 4, NULL);
	cv::putText(original, text, cv::Point(original.size().width / 2 - txtSz.width / 2, original.size().height /2 - 2* txtSz.height), cv::FONT_HERSHEY_PLAIN, 4, cv::Scalar(0, 0, 255), 4);
                
	char buff[3];
	sprintf(buff, "%d", ADAPTATION_TIME_SEC - abs(time(NULL) - camAdaptationStartTime));
	txtSz = cv::getTextSize(buff, cv::FONT_HERSHEY_PLAIN, 4, 4, NULL);
	cv::putText(original, buff, cv::Point(original.size().width / 2 - txtSz.width / 2, original.size().height /2 ), cv::FONT_HERSHEY_PLAIN, 4, cv::Scalar(0, 0, 255), 4);
      }
    }

    imshow("Tracking", original);

    int key = waitKey(30);
    if( (key & 0xFF) == 27)
      exit(0);
    else if ((key & 0xFF) == ' '){
      camAdapted = false;
      EnableCameraAutoAdjust(GetVideoNum(argc, argv));
      camAdaptationStartTime = time(NULL);
    }

  }


  return 0;
}
