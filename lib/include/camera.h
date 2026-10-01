#ifndef CAMERA_H
#define CAMERA_H
class Camera
{
private:
    void* handle;
public:
    Camera();
    ~Camera();
    bool open();
    bool close();
    bool setTriggerMode(bool enable);
    bool startGrabbing();
    bool stopGrabbing();
};
#endif