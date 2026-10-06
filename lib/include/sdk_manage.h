#ifndef SDK_MANAGE_H
#define SDK_MANAGE_H
class sdk_manage
{
private:
    bool initialized = false;
public:
    sdk_manage();
    ~sdk_manage();
    bool isInitialized() const;
};
#endif