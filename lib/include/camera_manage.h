#pragma once

#include <MvCameraControl.h>

class CameraManager
{
private:
    MV_CC_DEVICE_INFO_LIST m_device_list{};

public:
    CameraManager();
    ~CameraManager();
    bool enumerate_devices();
    MV_CC_DEVICE_INFO* select_device();
    MV_CC_DEVICE_INFO* get_device_info(unsigned int device_number);
};
