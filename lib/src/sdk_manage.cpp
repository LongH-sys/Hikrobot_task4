#include "sdk_manage.h"
#include <iostream>
#include "MvCameraControl.h"
sdk_manage::sdk_manage()
{
    int nRet = MV_CC_Initialize();
    if (nRet != MV_OK)
    {
        std::cerr <<"SDK初始化失败"<<std::endl;
    }
}
    sdk_manage::~sdk_manage()
{
    MV_CC_Finalize();
}


