#ifndef macVideoDev_h
#define macVideoDev_h

#ifdef __gnu_linux__
#include <linux/videodev2.h>

#else
#include <stdint.h>

#define V4L2_CTRL_CLASS_CAMERA          0x009a0000      /* Camera class controls */
#define V4L2_CID_CAMERA_CLASS_BASE      (V4L2_CTRL_CLASS_CAMERA | 0x900)

#define V4L2_CID_EXPOSURE_AUTO                  (V4L2_CID_CAMERA_CLASS_BASE+1)
enum  v4l2_exposure_auto_type {
       V4L2_EXPOSURE_AUTO = 0,
       V4L2_EXPOSURE_MANUAL = 1,
       V4L2_EXPOSURE_SHUTTER_PRIORITY = 2,
       V4L2_EXPOSURE_APERTURE_PRIORITY = 3
};

#define V4L2_CTRL_CLASS_USER            0x00980000      /* Old-style 'user' controls */
#define V4L2_CID_BASE                   (V4L2_CTRL_CLASS_USER | 0x900)
#define V4L2_CID_AUTO_WHITE_BALANCE     (V4L2_CID_BASE+12)


#define VIDIOC_S_CTRL		_IOWR('V', 28, struct v4l2_control)


typedef int32_t __s32;
typedef uint32_t __u32;

struct v4l2_control{
  __u32 id;
  __s32 value;
};

#endif

#endif
