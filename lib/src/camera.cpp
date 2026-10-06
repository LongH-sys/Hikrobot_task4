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
    bool success = true;
    if (grabbing)
    {
        if (!stopGrabbing())
        {
            success = false;
        }
    }
    if (opened)
    {
        int nRet = MV_CC_CloseDevice(handle);
        if (nRet != MV_OK)
        {
            std::cerr <<"关闭相机失败"<<std::endl;
            success = false;
        }
        else
        {
            opened = false;
        }
    }
    int nRet = MV_CC_DestroyHandle(handle);
    if (nRet != MV_OK)
    {
        std::cerr <<"销毁句柄失败"<<std::endl;
        return false;
    }
    handle = nullptr;
    opened = false;
    grabbing = false;
    softwareTriggerEnabled = false;
    return success;
}
bool Camera::open()
{
    if (opened)
    {
        return true;
    }
    if (info == nullptr)
    {
        std::cerr <<"设备信息为空"<<std::endl;
        return false;
    }
    if (handle != nullptr)
    {
        std::cerr <<"旧句柄尚未清理,请先关闭"<<std::endl;
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
        int nRet = MV_CC_DestroyHandle(handle);
        if (nRet == MV_OK)
        {
            handle = nullptr;
        }
        else
        {
            std::cerr <<"销毁句柄失败"<<std::endl;
        }
        return false;
    }
    opened = true;
    return true;
}
bool Camera::configureTriggerMode()
{
    int choice;
    std::cout <<"选择采集模式(1:软件触发,0:连续采集)"<<std::endl;
    if (!(std::cin >>choice) || (choice != 0 && choice != 1))
    {
        std::cerr <<"请输入0或1"<<std::endl;
        return false;
    }
    return setTriggerMode(choice == 1);
}
bool Camera::setTriggerMode(bool enable)
{
    if (!opened)
    {
        std::cerr <<"相机尚未打开"<<std::endl;
        return false;
    }
    if (grabbing)
    {
        std::cerr <<"请先停止采集,再修改触发模式"<<std::endl;
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
    softwareTriggerEnabled = false;
    if (enable)
    {
        nRet = MV_CC_SetEnumValue(handle, "TriggerSource", MV_TRIGGER_SOURCE_SOFTWARE);
        if (nRet != MV_OK)
        {
            std::cerr <<"设置软件触发失败"<<std::endl;
            return false;
        }
        softwareTriggerEnabled = true;
    }
    return true;
}
void __stdcall Camera::imageCallback(unsigned char* pData,MV_FRAME_OUT_INFO_EX* pFrameInfo,void* pUser)
{
    if (pData == nullptr || pFrameInfo == nullptr || pUser == nullptr)
    {
        return;
    }
    cv::Mat bgr;
    if (pFrameInfo->enPixelType == PixelType_Gvsp_RGB8_Packed)
    {
        cv::Mat rgb(pFrameInfo->nHeight,pFrameInfo->nWidth,CV_8UC3,pData);
        cv::cvtColor(rgb,bgr,cv::COLOR_RGB2BGR);
    }
    else if (pFrameInfo->enPixelType == PixelType_Gvsp_BayerRG8)
    {
        cv::Mat bayer(pFrameInfo->nHeight,pFrameInfo->nWidth,CV_8UC1,pData);
        cv::cvtColor(bayer,bgr,cv::COLOR_BayerRGGB2BGR);
    }
    else
    {
        std::cerr <<"暂不支持此像素格式:"<<pFrameInfo->enPixelType<<std::endl;
        return;
    }
    Camera* camera = static_cast<Camera*>(pUser);
    std::lock_guard<std::mutex> lock(camera->frameMutex);
    camera->latestFrame = bgr;
}
bool Camera::triggeronce()
{
    if (!opened || !grabbing)
    {
        std::cerr <<"请先打开相机并开始采集"<<std::endl;
        return false;
    }
    if (!softwareTriggerEnabled)
    {
        std::cerr <<"当前没有启用软件触发"<<std::endl;
        return false;
    }
    int nRet = MV_CC_SetCommandValue(handle, "TriggerSoftware");
    if (nRet != MV_OK)
    {
        std::cerr <<"发送软件触发命令失败,错误码:"<<nRet<<std::endl;
        return false;
    }
    return true;
}
bool Camera::startGrabbing()
{
    if (!opened)
    {
        std::cerr <<"相机尚未打开"<<std::endl;
        return false;
    }
    if (grabbing)
    {
        return true;
    }
    int nRet = MV_CC_RegisterImageCallBackEx(handle,imageCallback,this);
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
    grabbing = true;
    return true;
}
void Camera::waitForStop()
{
    cv::namedWindow("Camera",cv::WINDOW_NORMAL);
    if (softwareTriggerEnabled)
    {
        std::cout <<"请在图像窗口按t触发,按q停止触发"<<std::endl;
    }
    else
    {
        std::cout <<"连续采集中,请在图像窗口按q停止采集"<<std::endl;
    }
    while (true)
    {
        cv::Mat image;
        {
            std::lock_guard<std::mutex> lock(frameMutex);
            image = latestFrame.clone();
        }
        if (!image.empty())
        {
            cv::imshow("Camera",image);
        }
        int key = cv::waitKey(10);
        if (key == 'q' || key == 'Q')
        {
            break;
        }
        if (key == 't' || key == 'T')
        {
            if (!triggeronce())
            {
                std::cerr <<"本次触发未成功,可处理后重试"<<std::endl;
            }
        }
    }
    cv::destroyAllWindows();
}
bool Camera::stopGrabbing()
{
    if (!grabbing)
    {
        return true;
    }
    int nRet = MV_CC_StopGrabbing(handle);
    if (nRet != MV_OK)
    {
        std::cerr <<"停止采集失败"<<std::endl;
        return false;
    } 
    grabbing = false;
    std::cout <<"停止采集成功"<<std::endl;
    return true;
}