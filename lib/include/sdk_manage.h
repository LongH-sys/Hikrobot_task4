#pragma once

class SdkManager
{
private:
    bool m_initialized = false;

public:
    bool initialize();
    bool finalize();
};
