#ifndef CAMERA_H
#define CAMERA_H
#include "MvCameraControl.h"
#include <opencv2/core.hpp>
#include <mutex>
class Camera
{
private:
    void* handle;
    MV_CC_DEVICE_INFO* info;
    static void __stdcall imageCallback(unsigned char* pData,MV_FRAME_OUT_INFO_EX* pFrameInfo,void* pUser);
    cv::Mat latestFrame;
    std::mutex frameMutex;
public:
    Camera(MV_CC_DEVICE_INFO* info);
    ~Camera();
    bool open();
    bool close();
    bool configureTriggerMode();
    bool setTriggerMode(bool enable);
    bool startGrabbing();
    void waitForStop();
    bool stopGrabbing();
};
#endif