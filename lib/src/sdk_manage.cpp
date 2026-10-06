#include <iostream>

#include <MvCameraControl.h>

#include <sdk_manage.h>

SdkManager::SdkManager()
{
    int result = MV_CC_Initialize();
    if (result != MV_OK)
    {
        std::cerr << "SDK初始化失败" << std::endl;
        return;
    }
    m_initialized = true;
}

bool SdkManager::is_initialized() const
{
    return m_initialized;
}

SdkManager::~SdkManager()
{
    if (m_initialized)
    {
        MV_CC_Finalize();
    }
}
