#include <opencv2/opencv.hpp>
#include <unistd.h>
#include "proc/Util.hpp"

#define vOpenCV 1  /* 0 = OpenCV2; 1 = OpenCV3 */

int main(int argc, char **argv) {
    cv::Mat frame;
    cv::Mat fore;
    cv::Mat foreActual;
    cv::Mat work;
    
    DisableCameraAutoAdjust(GetVideoNum(argc, argv));
    cv::VideoCapture cap(GetVideoNum(argc, argv));
#if vOpenCV
	cv::Ptr<cv::BackgroundSubtractorMOG2> bg = cv::createBackgroundSubtractorMOG2();
#else
	cv::BackgroundSubtractorMOG2 bg = cv::BackgroundSubtractorMOG2();
#endif    

    int height = 150;
    while(true) {
        std::vector<std::vector<cv::Point> > contours;
        std::vector<std::vector<cv::Point> > contoursActual;
        std::vector<cv::Rect> rectangles;

        cap >> frame;

        cv::Rect InterestArea(0, 0, frame.size().width, height);
         
        cv::erode( frame, work, cv::Mat());
        cv::dilate( work, work, cv::Mat());

#if vOpenCV
        bg->apply(work, fore, 0.0007);
#else
        bg(work, fore, 0.0007);
#endif

        cv::imshow("Fore", fore);

        cv::Canny( fore, fore, 30, 150,  3 );
        cv::dilate( fore, fore, cv::Mat());

        fore(InterestArea).copyTo(foreActual);

        findContours( fore, contours,  CV_RETR_EXTERNAL , CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
        findContours( foreActual, contoursActual,  CV_RETR_EXTERNAL , CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );

        cv::Mat drawing = cv::Mat::zeros( fore.size(), CV_8UC3 );
        for(int i = 0; i< contours.size(); i++) {
            cv::Scalar color(0,0,255);
            cv::drawContours( drawing, contours, i, color, 2, 8);
        }
        
        std::vector<std::vector<cv::Point> >hull( contours.size() );
        for( int i = 0; i < contours.size(); i++ )
        {  
            if (cv::contourArea(contours[i]) > 1000)
                cv::convexHull( cv::Mat(contours[i]), hull[i] );
        }
    
        std::vector<std::vector<cv::Point> >hullActual( contoursActual.size() );
        for( int i = 0; i < contoursActual.size(); i++ )
        {  
            if (cv::contourArea(contoursActual[i]) > 500)
                cv::convexHull( cv::Mat(contoursActual[i]), hullActual[i] );
        }
        
        cv::line(frame, cv::Point(0, height), cv::Point(frame.size().width, height), cv::Scalar(0, 255, 0), 2);
        
        int counter = 0;
        for (auto single_hull : hullActual) {
            for (auto point: single_hull) {
                    if (point.y < height && point.y > height - 10) {
                        counter++;
                        break;
                    }
            }
        }
        char id[10];
        sprintf(id, "%02d", counter);
        cv::putText(frame, std::string(id),  cv::Point(2, 30), CV_FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
    
        
        for( int i = 0; i < hull.size(); i++ )
        {  
            cv::drawContours( drawing, hull, i, cv::Scalar(0, 255, 0), 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
        }
        

        cv::imshow("Frame", frame);
        cv::imshow("Hull and Contours", drawing );
            
        int key = cv::waitKey(30);
        if ((key & 0xFF) == 27) {
            break;
        }
        if ((key & 0xFFFF) == 0xFF52) {
            height -= 3;
            if (height < 0) {
                height = 0;
            }
        }
        if ((key & 0xFFFF) == 0xFF54) {
            height += 4;
            if (height > frame.size().height) {
                height = frame.size().height;
            }
        }
        else if ((key & 0xFF) == 'c') {
                imwrite("out.png", frame);
                system("python proc/sendmail.py");
        }
    }
    
    return 0;
}
