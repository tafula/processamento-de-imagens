#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include "Util.hpp"

ScreenSize GetScreenSize () {
    ScreenSize retVal;
    
#ifdef __gnu_linux__
    Display* disp = XOpenDisplay(NULL);
    Screen*  scrn = DefaultScreenOfDisplay(disp);
    
    retVal.height = scrn->height;
    retVal.width  = scrn->width;
#endif
    return retVal;
}

unsigned int GetVideoNum(int argc, char **argv) {
    int cam = 0;
    if (argc > 1) {
        cam = atoi(*(argv + 1));
    }

    return cam;    
}

#include <fcntl.h>
#include <sys/ioctl.h>
#include "macVideoDev.h"
#include <unistd.h>


void EnableCameraAutoAdjust(int devNum) {
    struct v4l2_control ctrl;
    char devName[25];
    
    sprintf(devName, "/dev/video%d", devNum);
    int cd = open(devName, O_RDWR);
    
    ctrl.id = V4L2_CID_EXPOSURE_AUTO ;
    ctrl.value = V4L2_EXPOSURE_APERTURE_PRIORITY ;
    ioctl(cd, VIDIOC_S_CTRL, &ctrl);
    
    ctrl.id = V4L2_CID_AUTO_WHITE_BALANCE;
    ctrl.value = true ;    
    ioctl(cd, VIDIOC_S_CTRL, &ctrl);   
    
    close(cd);
}


void DisableCameraAutoAdjust(int devNum) {
    struct v4l2_control ctrl;
    char devName[25];
    
    sprintf(devName, "/dev/video%d", devNum);
    int cd = open(devName, O_RDWR);
    
    ctrl.id = V4L2_CID_EXPOSURE_AUTO ;
    ctrl.value = V4L2_EXPOSURE_MANUAL;
    ioctl(cd, VIDIOC_S_CTRL, &ctrl);
    
    ctrl.id = V4L2_CID_AUTO_WHITE_BALANCE;
    ctrl.value = false ;    
    ioctl(cd, VIDIOC_S_CTRL, &ctrl);   
    
    close(cd);
}
