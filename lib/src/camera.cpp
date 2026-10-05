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
    int nRet = MV_CC_CloseDevice(handle);
    bool success = true;
    if (nRet != MV_OK)
    {
        std::cerr <<"关闭相机失败"<<std::endl;
        success = false;
    }
    nRet = MV_CC_DestroyHandle(handle);
    if (nRet != MV_OK)
        {
            std::cerr <<"销毁句柄失败"<<std::endl;
            return false;
        }
    handle = nullptr;
    return success;
}
bool Camera::open()
{
    if (info == nullptr)
    {
        std::cerr <<"设备信息为空"<<std::endl;
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
        return false;
    }
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
    if (pData == nullptr || pFrameInfo == nullptr)
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
}
bool Camera::startGrabbing()
{
    if (handle == nullptr)
    {
        std::cerr <<"相机句柄为空"<<std::endl;
        return false;
    }
    int nRet = MV_CC_RegisterImageCallBackEx(handle,imageCallback,nullptr);
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
    return true;
}
void Camera::waitForStop()
{
    std::cout <<"正在采集,输入q并回车停止"<<std::endl;
    char command;
    while (std::cin >>command)
    {
        if (command == 'q')
        {
            return;
        }
    }
}
bool Camera::stopGrabbing()
{
    if (handle == nullptr)
    {
        return false;
    }
    int nRet = MV_CC_StopGrabbing(handle);
    if (nRet != MV_OK)
    {
        std::cerr <<"停止采集失败"<<std::endl;
        return false;
    } 
    std::cout <<"停止采集成功"<<std::endl;
    return true;
}
