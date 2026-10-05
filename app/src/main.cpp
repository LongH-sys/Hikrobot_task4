#include "camera.h"
#include "sdk_manage.h"
#include "camera_manage.h"
#include <iostream>
int main()
{
    sdk_manage sdk;
    Camera_Manage cam_manage;
    Camera camera(cam_manage.selectDevice());
    if (!camera.open())
    {
        return -1;
    }
    if (!camera.configureTriggerMode())
    {
        return -1;
    }
    if (!camera.startGrabbing())
    {
        return -1;
    }
    camera.waitForStop();
    if (!camera.stopGrabbing())
    {
        return -1;
    }
    return 0;
}