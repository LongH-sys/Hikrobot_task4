#pragma once

class SdkManager
{
private:
    bool m_initialized = false;

public:
    SdkManager();
    SdkManager(const SdkManager&) = delete;
    SdkManager& operator=(const SdkManager&) = delete;
    ~SdkManager();
    bool is_initialized() const;
};
