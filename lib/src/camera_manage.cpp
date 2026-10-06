#include <iostream>

#include <camera_manage.h>

CameraManager::CameraManager()
{
}

CameraManager::~CameraManager()
{
}

bool CameraManager::enumerate_devices()
{
    int result = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &m_device_list);
    if (result != MV_OK)
    {
        std::cerr << "设备枚举失败" << std::endl;
        return false;
    }
    if (m_device_list.nDeviceNum > 0)
    {
        std::cout << "发现设备数量:" << m_device_list.nDeviceNum << std::endl;
        for (unsigned int i = 0; i < m_device_list.nDeviceNum; ++i)
        {
            MV_CC_DEVICE_INFO* info = m_device_list.pDeviceInfo[i];
            std::cout << "设备" << i + 1 << ":";
            if (info == nullptr)
            {
                std::cout << "设备信息为空" << std::endl;
                continue;
            }
            if (info->nTLayerType == MV_GIGE_DEVICE)
            {
                std::cout
                    << "Gige"
                    << ",型号:"
                    << info->SpecialInfo.stGigEInfo.chModelName
                    << ",序列号:"
                    << info->SpecialInfo.stGigEInfo.chSerialNumber
                    << std::endl;
            }
            else if (info->nTLayerType == MV_USB_DEVICE)
            {
                std::cout
                    << "USB"
                    << ",型号:"
                    << info->SpecialInfo.stUsb3VInfo.chModelName
                    << ",序列号"
                    << info->SpecialInfo.stUsb3VInfo.chSerialNumber
                    << std::endl;
            }
            else
            {
                std::cout << "暂不支持显示此设备类型的信息" << std::endl;
            }
        }
    }
    else
    {
        std::cerr << "没有找到设备" << std::endl;
        return false;
    }
    return true;
}

MV_CC_DEVICE_INFO* CameraManager::select_device()
{
    if (!enumerate_devices())
    {
        return nullptr;
    }
    unsigned int device_number = 0;
    std::cout << "请选择设备(1~" << m_device_list.nDeviceNum << "):";
    if (!(std::cin >> device_number))
    {
        std::cerr << "输入无效" << std::endl;
        return nullptr;
    }
    MV_CC_DEVICE_INFO* info = get_device_info(device_number);
    if (info == nullptr)
    {
        std::cerr << "设备编号超出范围" << std::endl;
    }
    return info;
}

MV_CC_DEVICE_INFO* CameraManager::get_device_info(unsigned int device_number)
{
    if (device_number == 0 || device_number > m_device_list.nDeviceNum)
    {
        return nullptr;
    }
    return m_device_list.pDeviceInfo[device_number - 1];
}
