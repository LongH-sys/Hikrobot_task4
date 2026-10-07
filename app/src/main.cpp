#include <camera.h>

#include <camera_manage.h>

#include <sdk_manage.h>

int main()
{
    SdkManager sdk;
    if (!sdk.initialize())
    {
        return -1;
    }
    CameraManager cam_manage;
    Camera camera(cam_manage.select_device());
    if (!camera.open())
    {
        if (camera.close())
        {
            sdk.finalize();
        }
        return -1;
    }
    if (!camera.configure_trigger_mode())
    {
        if (camera.close())
        {
            sdk.finalize();
        }
        return -1;
    }
    if (!camera.start_grabbing())
    {
        if (camera.close())
        {
            sdk.finalize();
        }
        return -1;
    }
    camera.wait_for_stop();
    if (!camera.stop_grabbing())
    {
        if (camera.close())
        {
            sdk.finalize();
        }
        return -1;
    }
    if (!camera.close())
    {
        return -1;
    }
    if (!sdk.finalize())
    {
        return -1;
    }
    return 0;
}
