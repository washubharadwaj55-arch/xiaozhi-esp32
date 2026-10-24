#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <string>
#include <memory>
#include <cstdint>
#include <cstddef>
#include <vector>

class LvglFont;
class LvglTheme;

class Theme {
public:
    Theme() = default;
    Theme(const std::string& name) : name_(name) {}
    virtual ~Theme() = default;

    virtual const std::string& name() const { return name_; }
    virtual const std::string& GetName() const { return name_; }
    virtual std::shared_ptr<LvglFont> GetTextFont() const { return nullptr; }

protected:
    std::string name_;
};

class Display {
public:
    virtual ~Display() = default;

    virtual int width() const { return width_; }
    virtual int height() const { return height_; }

    virtual bool SetStatus(const char* status) = 0;
    virtual bool ShowNotification(const char* notification, int duration_ms = 3000) = 0;
    virtual bool ShowNotification(const std::string& notification, int duration_ms = 3000) {
        return ShowNotification(notification.c_str(), duration_ms);
    }
    virtual bool SetEmotion(const char* emotion) = 0;
    virtual bool SetIcon(const char* icon) = 0;

    virtual void SetChatMessage(const char* role, const char* content) { (void)role; (void)content; }
    virtual void RemoveChatMessage(const char* role, const char* content) { (void)role; (void)content; }
    virtual void ClearChatMessages() {}
    virtual void SetHideSubtitle(bool hide) { (void)hide; }

    virtual bool SetTheme(const char* theme) { (void)theme; return true; }
    virtual bool SetTheme(std::shared_ptr<const Theme> theme) { (void)theme; return true; }
    virtual void SetTheme(LvglTheme* theme) { (void)theme; }
    virtual void SetTheme(Theme* theme) { (void)theme; }
    virtual Theme* GetTheme() const { return nullptr; }

    virtual bool SetImage(const uint8_t* data, size_t size) { (void)data; (void)size; return true; }
    virtual bool SnapshotToJpeg(std::string path, int quality) { (void)path; (void)quality; return true; }
    virtual bool SupportsEmoji(int code) { (void)code; return false; }
    virtual void UpdateStatusbar(bool update_all = false) { (void)update_all; }

    virtual void SetupUI() {}

    virtual bool IsMonochrome() const { return false; }
    virtual void SetPowerSaveMode(bool on) { (void)on; }

    virtual bool Lock(int timeout_ms = 0) = 0;
    virtual void Unlock() = 0;

protected:
    int width_ = 0;
    int height_ = 0;
};

#endif // _DISPLAY_H_
