#include <opencv2/opencv.hpp>
#include <unistd.h>
#include <sys/time.h>
#include "proc/Util.hpp"
#include "proc/InteractiveObject.hpp"
#include "proc/BlobDetector.hpp"

#define EROSION_DILATION_SIZE 5

using namespace cv;
using namespace std;

int main(int argc, char **argv) {
 //preconfigura (abre a camera)
  Mat frame;
  Mat orig;
  Mat fore;

  time_t camAdaptationStartTime = time(NULL);
  bool camAdapted = false;
    
  vector <vector<Point>> contours; //coisas que vao ser identificadas
    
  EnableCameraAutoAdjust(GetVideoNum(argc, argv));
  VideoCapture cap(GetVideoNum(argc, argv));

  BlobDetector detector(cv::MORPH_RECT, cv::Size(2 * EROSION_DILATION_SIZE + 1, 2 * EROSION_DILATION_SIZE + 1), cv::Point(EROSION_DILATION_SIZE, EROSION_DILATION_SIZE), 400);
    
  cvNamedWindow("Dial", CV_WINDOW_NORMAL);
  cvSetWindowProperty("Dial", CV_WND_PROP_AUTOSIZE, CV_WINDOW_AUTOSIZE);
  ScreenSize ss = GetScreenSize();
  cvResizeWindow("Dial", ss.width, ss.height);

  cap >> frame;
  //abriu a camera

  //fdsfsd
  const int squareSize = 50;
  const int squareMargin = 12;
  int iStepBox = 10;

  InteractiveObject *boxes[10];

  for (int i = 0; i < sizeof(boxes) / sizeof(boxes[0]); i++) {
    boxes[i] = new InteractiveObject(Rect( (squareSize + squareMargin )* (i)  + squareMargin , squareMargin, squareSize, squareSize));
  }

  char numbers[80];
  memset(numbers, 0, sizeof(numbers));
  while(true) {
    cap >> frame;
    flip(frame, frame, 1);
    frame.copyTo(orig);
        
    if (camAdapted) {
      Rect roi(0, 0, frame.size().width, 150);
      Mat cropped;

      detector.morphologyOperations(frame, frame);
	    
      frame(roi).copyTo(cropped);

      detector.findBlobs(cropped);
      detector.drawBlobs(orig, Scalar(255, 0, 255));
      int counter = detector.getBlobs().size();
            
      bool leftActive = false;
      bool rightActive = false;

      char buff[3];
      int i = 0;
      int activeBoxes = 0;
      for (auto box: boxes) {
	bool pressed = box->ProcessHulls(detector.getBlobs());
	if (pressed) {
	  sprintf(buff, "%d", i++);
	  strcat(numbers, buff);
	}
            
	if (box->IsActive()) {
	  activeBoxes++;
	}
	i++;
      }

      i = 0;
      for (auto box: boxes) {
	box->Draw(orig);
	int xText = (squareSize + squareMargin) * i + squareSize/2 -10;
	int yText = squareSize + squareMargin - 4;
	sprintf(buff, "%d", i++);
	putText(orig, buff, Point(xText, yText), FONT_HERSHEY_PLAIN, 4, Scalar(0 , 255, 255), 4, CV_AA );

      }
      if (activeBoxes > 1 ) {
	for (auto box: boxes) {
	  box->Deactivate();
	}
      }
      putText(orig, numbers, Point(10, orig.size().height-20), FONT_HERSHEY_PLAIN, 4, Scalar(0 , 255, 255), 4, CV_AA );
    }
    else {
      if ((time(NULL) - camAdaptationStartTime) > ADAPTATION_TIME_SEC) {
	camAdapted = true;
	detector.resetBackground();
	DisableCameraAutoAdjust(GetVideoNum(argc, argv));
      }
      else {
	string text = "Configurando...";
	Size txtSz = getTextSize(text, FONT_HERSHEY_PLAIN, 4, 4, NULL);
	putText(orig, text, Point(orig.size().width / 2 - txtSz.width / 2, orig.size().height /2 - 2* txtSz.height), FONT_HERSHEY_PLAIN, 4, Scalar(0, 0, 255), 4);
                
	char buff[3];
	sprintf(buff, "%d", ADAPTATION_TIME_SEC - abs(time(NULL) - camAdaptationStartTime));
	txtSz = getTextSize(buff, FONT_HERSHEY_PLAIN, 4, 4, NULL);
	putText(orig, buff, Point(orig.size().width / 2 - txtSz.width / 2, orig.size().height /2 ), FONT_HERSHEY_PLAIN, 4, Scalar(0, 0, 255), 4);
      }
    }

    if (strcmp(numbers, "1234") == 0) exit(0);
                

    imshow("Dial", orig);
    int key = waitKey(30);
    if ((key & 0xFF) == 27) {
      exit(0);
    }
    else if ((key & 0xFF) == ' '){
      camAdapted = false;
      EnableCameraAutoAdjust(GetVideoNum(argc, argv));   
      camAdaptationStartTime = time(NULL);
    }
    else if ((key & 0xFF) == 'c') {
      imwrite("out.png", orig);
      system("python proc/sendmail.py");
    }
  }
    
  return 0;
}

