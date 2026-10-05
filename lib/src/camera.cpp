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
        std::cerr <<"相机尚未打开"<<std::endl;
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

