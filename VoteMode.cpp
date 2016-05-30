#include <opencv2/opencv.hpp>
#include <unistd.h>
#include <sys/time.h>
#include "proc/Util.hpp"
#include "proc/InteractiveObject.hpp"
#include "proc/BlobDetector.hpp"

#define EROSION_DILATION_SIZE 5

int main(int argc, char **argv) {
  cv::Mat frame;
  cv::Mat orig;
  cv::Mat fore;
    
  time_t camAdaptationStartTime = time(NULL);
  bool camAdapted = false;

  BlobDetector detector(cv::MORPH_RECT, cv::Size(2 * EROSION_DILATION_SIZE + 1, 2 * EROSION_DILATION_SIZE + 1), cv::Point(EROSION_DILATION_SIZE, EROSION_DILATION_SIZE), 600);
    
  EnableCameraAutoAdjust(GetVideoNum(argc, argv));
  cv::VideoCapture cap(GetVideoNum(argc, argv));
    
  cvNamedWindow("Contador de Votos", CV_WINDOW_NORMAL);
  cvSetWindowProperty("Contador de Votos", CV_WND_PROP_AUTOSIZE, CV_WINDOW_AUTOSIZE);
  ScreenSize ss = GetScreenSize();
  cvResizeWindow("Contador de Votos", ss.width, ss.height);

  cap >> frame;

  const int squareSize = 150;
  const int squareMargin = 10;
    
  cv::Point leftOrigin(squareMargin, squareMargin);
  cv::Point rightOrigin(frame.size().width - squareSize - squareMargin, squareMargin);

  InteractiveObject left(cv::Rect(leftOrigin.x, leftOrigin.y, squareSize, squareSize));
  InteractiveObject right(cv::Rect(rightOrigin.x, rightOrigin.y, squareSize, squareSize));

  cv::Mat yesc = cv::imread("imgs/YESC.png", CV_LOAD_IMAGE_COLOR);
  cv::resize(yesc, yesc, cv::Size(squareSize, squareSize));

  cv::Mat yesbw = cv::imread("imgs/YESBW.png", CV_LOAD_IMAGE_COLOR);
  cv::resize(yesbw, yesbw, cv::Size(squareSize, squareSize));

  cv::Mat noc = cv::imread("imgs/NOC.png", CV_LOAD_IMAGE_COLOR);
  cv::resize(noc, noc, cv::Size(squareSize, squareSize));

  cv::Mat nobw = cv::imread("imgs/NOBW.png", CV_LOAD_IMAGE_COLOR);
  cv::resize(nobw, nobw, cv::Size(squareSize, squareSize));

  while(true) {
    cap >> frame;
    cv::flip(frame, frame, 1);
    frame.copyTo(orig);
        
    if (camAdapted) {
      detector.morphologyOperations(frame, frame);

      cv::Mat cropped;
      cv::Rect roi(0, 0, frame.size().width, 150);        
      frame(roi).copyTo(cropped);
            
      detector.findBlobs(cropped);

      int counter = detector.getBlobs().size();
      detector.drawBlobs(orig, Scalar(255,0,255));
            
      bool leftActive = false;
      bool rightActive = false;
      
      left.ProcessHulls(detector.getBlobs());
      right.ProcessHulls(detector.getBlobs());
            
      if (!(left.IsActive() && right.IsActive())) {
	char buff[11];
	sprintf(buff, "%02d", left.GetCount());
	cv::putText(orig, buff, cv::Point(40, orig.size().height - 40), cv::FONT_HERSHEY_PLAIN, 4, cv::Scalar(0, 255, 0), 4);

	sprintf(buff, "%02d", right.GetCount());
	cv::Size txtSz = cv::getTextSize(buff, cv::FONT_HERSHEY_PLAIN, 4, 4, NULL);
	cv::putText(orig, buff, cv::Point(orig.size().width - txtSz.width - 40, orig.size().height - 40), cv::FONT_HERSHEY_PLAIN, 4, cv::Scalar(0, 255, 0), 4);

	left.Draw(orig);
	right.Draw(orig);
               
	if (right.IsCounted()) { 
	  yesc.copyTo(orig(cv::Rect(rightOrigin.x, rightOrigin.y, yesc.size().width, yesc.size().height)));
	}
	else {
	  yesbw.copyTo(orig(cv::Rect(rightOrigin.x, rightOrigin.y, yesbw.size().width, yesbw.size().height)));
	}

	if (left.IsCounted()) { 
	  noc.copyTo(orig(cv::Rect(leftOrigin.x, leftOrigin.y, noc.size().width, noc.size().height)));
	}
	else {
	  nobw.copyTo(orig(cv::Rect(leftOrigin.x, leftOrigin.y, nobw.size().width, nobw.size().height)));

	}
                
      }
      else {
	left.Deactivate();
	right.Deactivate();
      }
    }

    else {
      if ((time(NULL) - camAdaptationStartTime) > ADAPTATION_TIME_SEC) {
	camAdapted = true;
	detector.resetBackground();
	DisableCameraAutoAdjust(GetVideoNum(argc, argv));
      }
      else {
	std::string text = "Configurando...";
	cv::Size txtSz = cv::getTextSize(text, cv::FONT_HERSHEY_PLAIN, 4, 4, NULL);
	cv::putText(orig, text, cv::Point(orig.size().width / 2 - txtSz.width / 2, orig.size().height /2 - 2* txtSz.height), cv::FONT_HERSHEY_PLAIN, 4, cv::Scalar(0, 0, 255), 4);
                
	char buff[3];
	sprintf(buff, "%d", ADAPTATION_TIME_SEC - abs(time(NULL) - camAdaptationStartTime));
	txtSz = cv::getTextSize(buff, cv::FONT_HERSHEY_PLAIN, 4, 4, NULL);
	cv::putText(orig, buff, cv::Point(orig.size().width / 2 - txtSz.width / 2, orig.size().height /2 ), cv::FONT_HERSHEY_PLAIN, 4, cv::Scalar(0, 0, 255), 4);
      }
    }

    imshow("Contador de Votos", orig);
            
    int key = cv::waitKey(30);
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

