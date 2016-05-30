#ifndef UTIL_HPP
#define UTIL_HPP

#define ADAPTATION_TIME_SEC 3

typedef struct {
        int width;
        int height;    
} ScreenSize;

ScreenSize GetScreenSize();
    
unsigned int GetVideoNum(int argc, char **argv); 

void EnableCameraAutoAdjust(int devNum);
void DisableCameraAutoAdjust(int devNum);

#endif
