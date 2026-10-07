#include <iostream>

#include <MvCameraControl.h>

#include <sdk_manage.h>

bool SdkManager::initialize()
{
    if (m_initialized)
    {
        return true;
    }
    int result = MV_CC_Initialize();
    if (result != MV_OK)
    {
        std::cerr << "SDK初始化失败" << std::endl;
        return false;
    }
    m_initialized = true;
    return true;
}

bool SdkManager::finalize()
{
    if (!m_initialized)
    {
        return true;
    }
    int result = MV_CC_Finalize();
    if (result != MV_OK)
    {
        std::cerr <<"SDK反初始化失败"<<std::endl;
        return false;
    }
    m_initialized = false;
    return true;
}
