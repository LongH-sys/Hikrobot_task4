#include "camera.h"
#include <iostream>
#include "MvCameraControl.h"
Camera::Camera()
{
    handle = nullptr;
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
        int nRet = MV_CC_DestroyHandle(handle);
        if (nRet != MV_OK)
        {
            std::cerr <<"销毁句柄失败"<<std::endl;
            return false;
        }
        handle = nullptr;
        return true;
}

