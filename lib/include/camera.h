#pragma once

#include <mutex>

#include <opencv2/core.hpp>

#include <MvCameraControl.h>

class Camera
{
private:
    void* m_handle = nullptr;
    MV_CC_DEVICE_INFO* m_info = nullptr;
    static void __stdcall image_callback(
        unsigned char* data,
        MV_FRAME_OUT_INFO_EX* frame_info,
        void* user_data
    );
    cv::Mat m_latest_frame{};
    std::mutex m_frame_mutex{};
    bool m_opened = false;
    bool m_grabbing = false;
    bool m_software_trigger_enabled = false;

public:
    explicit Camera(MV_CC_DEVICE_INFO* info);
    Camera(const Camera&) = delete;
    Camera& operator = (const Camera&) = delete;
    ~Camera();
    bool open();
    bool close();
    bool configure_trigger_mode();
    bool set_trigger_mode(bool enable);
    bool start_grabbing();
    void wait_for_stop();
    bool stop_grabbing();
    bool trigger_once();
};
