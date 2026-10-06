#include <iostream>

#include <opencv2/opencv.hpp>

#include <camera.h>

Camera::Camera(MV_CC_DEVICE_INFO* info)
    : m_info(info)
{
}

Camera::~Camera()
{
    close();
}

bool Camera::close()
{
    if (m_handle == nullptr)
    {
        return true;
    }
    bool success = true;
    if (m_grabbing)
    {
        if (!stop_grabbing())
        {
            success = false;
        }
    }
    if (m_opened)
    {
        int result = MV_CC_CloseDevice(m_handle);
        if (result != MV_OK)
        {
            std::cerr << "关闭相机失败" << std::endl;
            success = false;
        }
        else
        {
            m_opened = false;
        }
    }
    int result = MV_CC_DestroyHandle(m_handle);
    if (result != MV_OK)
    {
        std::cerr << "销毁句柄失败" << std::endl;
        return false;
    }
    m_handle = nullptr;
    m_opened = false;
    m_grabbing = false;
    m_software_trigger_enabled = false;
    return success;
}

bool Camera::open()
{
    if (m_opened)
    {
        return true;
    }
    if (m_info == nullptr)
    {
        std::cerr << "设备信息为空" << std::endl;
        return false;
    }
    if (m_handle != nullptr)
    {
        std::cerr << "旧句柄尚未清理,请先关闭" << std::endl;
        return false;
    }
    int result = MV_CC_CreateHandle(&m_handle, m_info);
    if (result != MV_OK)
    {
        std::cerr << "创建句柄失败" << std::endl;
        return false;
    }
    result = MV_CC_OpenDevice(m_handle);
    if (result != MV_OK)
    {
        std::cerr << "打开设备失败" << std::endl;
        int destroy_result = MV_CC_DestroyHandle(m_handle);
        if (destroy_result == MV_OK)
        {
            m_handle = nullptr;
        }
        else
        {
            std::cerr << "销毁句柄失败" << std::endl;
        }
        return false;
    }
    m_opened = true;
    return true;
}

bool Camera::configure_trigger_mode()
{
    int choice = 0;
    std::cout << "选择采集模式(1:软件触发,0:连续采集)" << std::endl;
    if (!(std::cin >> choice) || (choice != 0 && choice != 1))
    {
        std::cerr << "请输入0或1" << std::endl;
        return false;
    }
    return set_trigger_mode(choice == 1);
}

bool Camera::set_trigger_mode(bool enable)
{
    if (!m_opened)
    {
        std::cerr << "相机尚未打开" << std::endl;
        return false;
    }
    if (m_grabbing)
    {
        std::cerr << "请先停止采集,再修改触发模式" << std::endl;
        return false;
    }
    int result = MV_OK;
    if (enable)
    {
        result = MV_CC_SetEnumValue(m_handle, "TriggerMode", MV_TRIGGER_MODE_ON);
    }
    else
    {
        result = MV_CC_SetEnumValue(m_handle, "TriggerMode", MV_TRIGGER_MODE_OFF);
    }
    if (result != MV_OK)
    {
        std::cerr << "设置触发模式失败" << std::endl;
        return false;
    }
    m_software_trigger_enabled = false;
    if (enable)
    {
        result = MV_CC_SetEnumValue(m_handle, "TriggerSource", MV_TRIGGER_SOURCE_SOFTWARE);
        if (result != MV_OK)
        {
            std::cerr << "设置软件触发失败" << std::endl;
            return false;
        }
        m_software_trigger_enabled = true;
    }
    return true;
}

void __stdcall Camera::image_callback(
    unsigned char* data,
    MV_FRAME_OUT_INFO_EX* frame_info,
    void* user_data
)
{
    if (data == nullptr || frame_info == nullptr || user_data == nullptr)
    {
        return;
    }
    cv::Mat bgr{};
    if (frame_info->enPixelType == PixelType_Gvsp_RGB8_Packed)
    {
        cv::Mat rgb(frame_info->nHeight, frame_info->nWidth, CV_8UC3, data);
        cv::cvtColor(rgb, bgr, cv::COLOR_RGB2BGR);
    }
    else if (frame_info->enPixelType == PixelType_Gvsp_BayerRG8)
    {
        cv::Mat bayer(frame_info->nHeight, frame_info->nWidth, CV_8UC1, data);
        cv::cvtColor(bayer, bgr, cv::COLOR_BayerRGGB2BGR);
    }
    else
    {
        std::cerr << "暂不支持此像素格式:" << frame_info->enPixelType << std::endl;
        return;
    }
    Camera* camera = static_cast<Camera*>(user_data);
    std::lock_guard<std::mutex> lock(camera->m_frame_mutex);
    camera->m_latest_frame = bgr;
}

bool Camera::trigger_once()
{
    if (!m_opened || !m_grabbing)
    {
        std::cerr << "请先打开相机并开始采集" << std::endl;
        return false;
    }
    if (!m_software_trigger_enabled)
    {
        std::cerr << "当前没有启用软件触发" << std::endl;
        return false;
    }
    int result = MV_CC_SetCommandValue(m_handle, "TriggerSoftware");
    if (result != MV_OK)
    {
        std::cerr << "发送软件触发命令失败,错误码:" << result << std::endl;
        return false;
    }
    return true;
}

bool Camera::start_grabbing()
{
    if (!m_opened)
    {
        std::cerr << "相机尚未打开" << std::endl;
        return false;
    }
    if (m_grabbing)
    {
        return true;
    }
    int result = MV_CC_RegisterImageCallBackEx(m_handle, image_callback, this);
    if (result != MV_OK)
    {
        std::cerr << "注册图像回调失败" << std::endl;
        return false;
    }
    result = MV_CC_StartGrabbing(m_handle);
    if (result != MV_OK)
    {
        std::cerr << "开始采集失败" << std::endl;
        return false;
    }
    m_grabbing = true;
    return true;
}

void Camera::wait_for_stop()
{
    cv::namedWindow("Camera", cv::WINDOW_NORMAL);
    if (m_software_trigger_enabled)
    {
        std::cout << "请在图像窗口按t触发,按q停止触发" << std::endl;
    }
    else
    {
        std::cout << "连续采集中,请在图像窗口按q停止采集" << std::endl;
    }
    while (true)
    {
        cv::Mat image{};
        {
            std::lock_guard<std::mutex> lock(m_frame_mutex);
            image = m_latest_frame.clone();
        }
        if (!image.empty())
        {
            cv::imshow("Camera", image);
        }
        int key = cv::waitKey(10);
        if (cv::getWindowProperty("Camera", cv::WND_PROP_AUTOSIZE) < 0)
        {
            break;
        }
        if (key == 'q' || key == 'Q')
        {
            break;
        }
        if (key == 't' || key == 'T')
        {
            if (!trigger_once())
            {
                std::cerr << "本次触发未成功,可处理后重试" << std::endl;
            }
        }
    }
    cv::destroyAllWindows();
}

bool Camera::stop_grabbing()
{
    if (!m_grabbing)
    {
        return true;
    }
    int result = MV_CC_StopGrabbing(m_handle);
    if (result != MV_OK)
    {
        std::cerr << "停止采集失败" << std::endl;
        return false;
    }
    m_grabbing = false;
    std::cout << "停止采集成功" << std::endl;
    return true;
}
