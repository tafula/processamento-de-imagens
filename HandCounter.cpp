#include<opencv2/opencv.hpp>
#include<unistd.h>
#include<sys/time.h>
#include "Util.hpp"
#include"BlobDetector.hpp"

#define EROSION_DILATION_SIZE 5

int main(int argc, char **argv) {
  cv::Mat frame;
  cv::Mat orig;
  cv::Mat fore;
    
  time_t camAdaptationStartTime = time(NULL);
  bool camAdapted = false;

  std::vector <std::vector<cv::Point>> contours;
    
  cvNamedWindow("Contador de Votos", CV_WINDOW_NORMAL);
  cvSetWindowProperty("Contador de Votos", CV_WND_PROP_AUTOSIZE, CV_WINDOW_AUTOSIZE);
  ScreenSize ss = GetScreenSize();
  cvResizeWindow("Contador de Votos", ss.width, ss.height);

   
  EnableCameraAutoAdjust(GetVideoNum(argc, argv));
  cv::VideoCapture cap(GetVideoNum(argc, argv));

  BlobDetector detector(cv::MORPH_RECT, cv::Size(2 * EROSION_DILATION_SIZE + 1, 2 * EROSION_DILATION_SIZE + 1), cv::Point(EROSION_DILATION_SIZE, EROSION_DILATION_SIZE), 600);
  
  while(true) {
    cap >> frame;
    cv::flip(frame, frame, 1);
    frame.copyTo(orig);
        
    if (camAdapted) {
      
      detector.morphologyOperations(frame, frame);
      
      cv::Rect roi(0, 0, frame.size().width, 150);
      
      cv::Mat cropped;
      frame(roi).copyTo(cropped);
      
      detector.findBlobs(cropped);

      detector.drawBlobs(orig, Scalar(255, 0, 255));
     
      char buff[11];
      sprintf(buff, "Votos: %02d", detector.getBlobs().size());

      cv::putText(orig, buff, cv::Point(20, orig.size().height - 40), cv::FONT_HERSHEY_PLAIN, 6, cv::Scalar(0, 255, 0), 4);
      cv::rectangle(orig, cv::Point(0, 0), cv::Point(frame.size().width, 150), cv::Scalar(255, 0, 0));
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
      system("python sendmail.py");
    }
 
  }
    
  return 0;
}
