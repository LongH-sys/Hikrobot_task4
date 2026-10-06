#include "camera_manage.h"
#include "MvCameraControl.h"
#include <iostream>
#include <cstring>
Camera_Manage::Camera_Manage()
{
    memset(&deviceList,0,sizeof(MV_CC_DEVICE_INFO_LIST));
}
Camera_Manage::~Camera_Manage()
{

}
bool Camera_Manage::camera_enum()
{
    int nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &deviceList);
    if (nRet != MV_OK)
    {
        std::cerr <<"设备枚举失败"<<std::endl;
        return false;
    }
    if (deviceList.nDeviceNum > 0)
    {
        std::cout <<"发现设备数量:"<<deviceList.nDeviceNum<<std::endl;
        for (unsigned int i=0;i < deviceList.nDeviceNum;++i)
        {
            MV_CC_DEVICE_INFO* info = deviceList.pDeviceInfo[i];
            std::cout <<"设备"<<i + 1<<":";
            if (info == nullptr)
            {
                std::cout <<"设备信息为空"<<std::endl;
                continue;
            }
            if (info->nTLayerType == MV_GIGE_DEVICE)
            {
                std::cout <<"Gige"<<",型号:"<<info->SpecialInfo.stGigEInfo.chModelName<<",序列号:"<<info->SpecialInfo.stGigEInfo.chSerialNumber<<std::endl;
            }
            else if (info->nTLayerType == MV_USB_DEVICE)
            {
                std::cout <<"USB"<<",型号:"<<info->SpecialInfo.stUsb3VInfo.chModelName<<",序列号"<<info->SpecialInfo.stUsb3VInfo.chSerialNumber<<std::endl;
            }
            else
            {
                std::cout <<"暂不支持显示此设备类型的信息"<<std::endl;
            }
        }
    }
    else
    {
        std::cerr <<"没有找到设备"<<std::endl;
        return false;
    }
    return true;
}
MV_CC_DEVICE_INFO* Camera_Manage::selectDevice()
{
    if (!camera_enum())
    {
        return nullptr;
    }
    unsigned int deviceNumber;
    std::cout <<"请选择设备(1~"<<deviceList.nDeviceNum<<"):";
    if (!(std::cin >>deviceNumber))
    {
        std::cerr <<"输入无效"<<std::endl;
        return nullptr;
    }
    MV_CC_DEVICE_INFO* info = getDeviceInfo(deviceNumber);
    if (info == nullptr)
    {
        std::cerr <<"设备编号超出范围"<<std::endl;
    }
    return info;
}
MV_CC_DEVICE_INFO* Camera_Manage::getDeviceInfo(unsigned int deviceNumber)
{
    if (deviceNumber == 0 || deviceNumber > deviceList.nDeviceNum)
    {
        return nullptr;
    }
    return deviceList.pDeviceInfo[deviceNumber-1];
}