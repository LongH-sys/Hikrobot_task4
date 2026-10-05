#ifndef CAMERA_MANAGE_H
#define CAMERA_MANAGE_H
#include "MvCameraControl.h"
class Camera_Manage
{
private:
    MV_CC_DEVICE_INFO_LIST deviceList{};

public:
    Camera_Manage();
    ~Camera_Manage();
    bool camera_enum();
    MV_CC_DEVICE_INFO* selectDevice();
    MV_CC_DEVICE_INFO* getDeviceInfo(unsigned int deviceNumber);
};
#endif