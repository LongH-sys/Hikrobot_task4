#include "camera.h"
#include <iostream>
#include "MvCameraControl.h"
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
    if (handle != nullptr)
    {
        int nRet = MV_CC_CloseDevice(handle);
        if (nRet != MV_OK)
        {
            std::cerr <<"关闭相机失败"<<std::endl;
            return false;
        }    
    }
    else
    {
        return true;
    }
    int nRet = MV_CC_DestroyHandle(handle);
    if (nRet != MV_OK)
        {
            std::cerr <<"销毁句柄失败"<<std::endl;
            return false;
        }
    handle = nullptr;
    return true;
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
        MV_CC_DestroyHandle(handle);
        handle = nullptr;
        return false;
    }
    return true;
}

