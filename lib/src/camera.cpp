#include "camera.h"
#include <iostream>
#include "MvCameraControl.h"
#include <opencv2/opencv.hpp>
Camera::Camera(MV_CC_DEVICE_INFO* info)
{
    handle = nullptr;
    this->info = info;
}
Camera::~Camera()
{
    close();
}
bool Camera::close()
{
    if (handle == nullptr)
    {
        return true;
    }
    bool success = true;
    if (grabbing)
    {
        if (!stopGrabbing())
        {
            success = false;
        }
    }
    if (opened)
    {
        int nRet = MV_CC_CloseDevice(handle);
        if (nRet != MV_OK)
        {
            std::cerr <<"关闭相机失败"<<std::endl;
            success = false;
        }
        else
        {
            opened = false;
        }
    }
    int nRet = MV_CC_DestroyHandle(handle);
    if (nRet != MV_OK)
    {
        std::cerr <<"销毁句柄失败"<<std::endl;
        return false;
    }
    handle = nullptr;
    opened = false;
    grabbing = false;
    return success;
}
bool Camera::open()
{
    if (opened)
    {
        return true;
    }
    if (info == nullptr)
    {
        std::cerr <<"设备信息为空"<<std::endl;
        return false;
    }
    if (handle != nullptr)
    {
        std::cerr <<"旧句柄尚未清理,请先关闭"<<std::endl;
        return false;
    }
    int nRet = MV_CC_CreateHandle(&handle, info);
    if (nRet != MV_OK)
    {
        std::cerr <<"创建句柄失败"<<std::endl;
        return false;
    }
    nRet = MV_CC_OpenDevice(handle);
    if (nRet != MV_OK)
    {
        std::cerr <<"打开设备失败"<<std::endl;
        int nRet = MV_CC_DestroyHandle(handle);
        if (nRet == MV_OK)
        {
            handle = nullptr;
        }
        else
        {
            std::cerr <<"销毁句柄失败"<<std::endl;
        }
        return false;
    }
    opened = true;
    return true;
}
bool Camera::configureTriggerMode()
{
    int choice;
    std::cout <<"是否开启触发模式?(1:开启,0:关闭)"<<std::endl;
    if (!(std::cin >>choice) || (choice != 0 && choice != 1))
    {
        std::cerr <<"请输入0或1"<<std::endl;
        return false;
    }
    return setTriggerMode(choice == 1);
}
bool Camera::setTriggerMode(bool enable)
{
    if (handle == nullptr)
    {
        std::cerr <<"相机句柄为空"<<std::endl;
        return false;
    }
    int nRet;
    if (enable)
    {
        nRet = MV_CC_SetEnumValue(handle, "TriggerMode", MV_TRIGGER_MODE_ON);
    }
    else
    {
        nRet = MV_CC_SetEnumValue(handle, "TriggerMode", MV_TRIGGER_MODE_OFF);
    }
    if (nRet != MV_OK)
    {
        std::cerr <<"设置触发模式失败"<<std::endl;
        return false;
    }
    return true;
}
void __stdcall Camera::imageCallback(unsigned char* pData,MV_FRAME_OUT_INFO_EX* pFrameInfo,void* pUser)
{
    if (pData == nullptr || pFrameInfo == nullptr || pUser == nullptr)
    {
        return;
    }
    if (pFrameInfo->enPixelType != PixelType_Gvsp_RGB8_Packed)
    {
        std::cerr <<"当前图像不是RGB8格式"<<std::endl;
        return;
    }
    cv::Mat rgb(pFrameInfo->nHeight,pFrameInfo->nWidth,CV_8UC3,pData);
    cv::Mat bgr;
    cv::cvtColor(rgb,bgr,cv::COLOR_RGB2BGR);
    Camera* camera = static_cast<Camera*>(pUser);
    std::lock_guard<std::mutex> lock(camera->frameMutex);
    camera->latestFrame = bgr;
}
bool Camera::startGrabbing()
{
    if (!opened)
    {
        std::cerr <<"相机尚未打开"<<std::endl;
        return false;
    }
    if (grabbing)
    {
        return true;
    }
    int nRet = MV_CC_RegisterImageCallBackEx(handle,imageCallback,this);
    if (nRet != MV_OK)
    {
        std::cerr <<"注册图像回调失败"<<std::endl;
        return false;
    }
    nRet = MV_CC_StartGrabbing(handle);
    if (nRet != MV_OK)
    {
        std::cerr <<"开始采集失败"<<std::endl;
        return false;
    }
    grabbing = true;
    return true;
}
void Camera::waitForStop()
{
    cv::namedWindow("Camera",cv::WINDOW_NORMAL);
    std::cout <<"请在图像窗口按q停止采集"<<std::endl;
    while (true)
    {
        cv::Mat image;
        {
            std::lock_guard<std::mutex> lock(frameMutex);
            image = latestFrame.clone();
        }
        if (!image.empty())
        {
            cv::imshow("Camera",image);
        }
        int key = cv::waitKey(10);
        if (key == 'q' || key == 'Q')
        {
            break;
        }
    }
    cv::destroyAllWindows();
}
bool Camera::stopGrabbing()
{
    if (!grabbing)
    {
        return true;
    }
    int nRet = MV_CC_StopGrabbing(handle);
    if (nRet != MV_OK)
    {
        std::cerr <<"停止采集失败"<<std::endl;
        return false;
    } 
    grabbing = false;
    std::cout <<"停止采集成功"<<std::endl;
    return true;
}