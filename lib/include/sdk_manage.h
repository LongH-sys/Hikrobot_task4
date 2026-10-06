#pragma once

class SdkManager
{
private:
    bool m_initialized = false;

public:
    SdkManager();
    ~SdkManager();
    bool is_initialized() const;
};
