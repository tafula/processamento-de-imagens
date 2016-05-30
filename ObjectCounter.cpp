#include <opencv2/opencv.hpp>
#include <unistd.h>
#include <sys/time.h>
#include "proc/Util.hpp"
#include "proc/BlobDetector.hpp"

#define EROSION_DILATION_SIZE 3

using namespace cv;
using namespace std;

int main(int argc, char** argv){
  Mat frame;
  Mat orig;
  Mat fore;

  time_t camAdaptationStartTime = time(NULL);
  bool camAdapted = false;

  vector< vector<Point> > objects;

  cvNamedWindow("Contador de Objetos", CV_WINDOW_NORMAL);
  cvSetWindowProperty("Contador de Objetos", CV_WND_PROP_AUTOSIZE, CV_WINDOW_AUTOSIZE);

  ScreenSize ss = GetScreenSize();
  cvResizeWindow("Contador de Objetos", ss.width, ss.height);

  EnableCameraAutoAdjust(GetVideoNum(argc, argv));
  VideoCapture cap(GetVideoNum(argc, argv));

  BlobDetector detector(MORPH_RECT, Size(2 * EROSION_DILATION_SIZE + 1, 2 * EROSION_DILATION_SIZE + 1), Point(EROSION_DILATION_SIZE, EROSION_DILATION_SIZE), 729, 10E6, 125, 255);

  while(true){
    cap >> frame;
    frame.copyTo(orig);
    
    if(camAdapted){
      detector.morphologyOperations(frame, frame);
      
      imshow("Treated Image", detector.findBlobs(frame));
      
      detector.drawBlobs(orig, Scalar(255,255,0));

      char buffer[11];
      sprintf(buffer, "%lu objetos", detector.getBlobs().size());

      putText(orig,buffer,Point(20,orig.size().height-40),FONT_HERSHEY_PLAIN,6,Scalar(0,0,0),4);
    }

    else{
      if ((time(NULL) - camAdaptationStartTime) > ADAPTATION_TIME_SEC) {
	camAdapted = true;
	detector.resetBackground();
	DisableCameraAutoAdjust(GetVideoNum(argc, argv));
      }
      else{
	string text = "Configurando...";
	Size txtSz = getTextSize(text, FONT_HERSHEY_PLAIN, 4, 4, NULL);
	putText(orig,text,Point(orig.size().width / 2 - txtSz.width / 2, orig.size().height /2 - 2* txtSz.height), FONT_HERSHEY_PLAIN, 4, Scalar(0, 0, 255), 4);
                
	char buff[3];
	sprintf(buff, "%ld", ADAPTATION_TIME_SEC - abs(time(NULL) - camAdaptationStartTime));
	txtSz = getTextSize(buff, FONT_HERSHEY_PLAIN, 4, 4, NULL);
	cv::putText(orig, buff, Point(orig.size().width / 2 - txtSz.width / 2, orig.size().height /2 ), cv::FONT_HERSHEY_PLAIN, 4, Scalar(0, 0, 255), 4);
      }
    }

    imshow("Contador de Objetos", orig);

    int key = cv::waitKey(30);
    if ((key & 0xFF) == 27) {
      exit(0);
    }
    else if ((key & 0xFF) == ' '){
      camAdapted = false;
      EnableCameraAutoAdjust(GetVideoNum(argc, argv));   
      camAdaptationStartTime = time(NULL);
    }
    
  }
}
