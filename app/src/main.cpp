#include <camera.h>
#include <camera_manage.h>
#include <sdk_manage.h>

int main()
{
    SdkManager sdk;
    if (!sdk.is_initialized())
    {
        return -1;
    }
    CameraManager cam_manage;
    Camera camera(cam_manage.select_device());
    if (!camera.open())
    {
        return -1;
    }
    if (!camera.configure_trigger_mode())
    {
        return -1;
    }
    if (!camera.start_grabbing())
    {
        return -1;
    }
    camera.wait_for_stop();
    if (!camera.stop_grabbing())
    {
        return -1;
    }
    return 0;
}
