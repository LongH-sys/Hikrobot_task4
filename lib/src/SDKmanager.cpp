#include "SDKmanager.h"
#include <iostream>
#include "MvCameraControl.h"
SDKManager::SDKManager()
{
    int nRet = MV_CC_Initialize();
    if (nRet != MV_OK)
    {
        std::cerr <<"SDK初始化失败"<<std::endl;
    }
}
    SDKManager::~SDKManager()
{
    MV_CC_Finalize();
}


