#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <string>
#include <memory>
#include <cstdint>
#include <cstddef>

class Theme;

class Display {
public:
    virtual ~Display() = default;

    virtual bool SetStatus(const char* status) = 0;
    virtual bool ShowNotification(const char* notification, int duration_ms = 3000) = 0;
    virtual bool ShowNotification(const std::string& notification, int duration_ms = 3000) {
        return ShowNotification(notification.c_str(), duration_ms);
    }
    virtual bool SetEmotion(const char* emotion) = 0;
    virtual bool SetIcon(const char* icon) = 0;
    virtual bool SetChatMessage(const char* role, const char* content) = 0;
    virtual bool ClearChatMessages() = 0;
    virtual bool SetTheme(const char* theme) = 0;
    virtual bool SetTheme(std::shared_ptr<const Theme> theme) = 0;
    virtual bool SetImage(const uint8_t* data, size_t size) = 0;
    virtual bool SnapshotToJpeg(std::string path, int quality) = 0;
    virtual bool SupportsEmoji(int code) { return false; }
    virtual void UpdateStatusbar(bool update_all = false) = 0;

protected:
    virtual bool Lock(int timeout_ms = 0) = 0;
    virtual void Unlock() = 0;
};

#endif // _DISPLAY_H_
