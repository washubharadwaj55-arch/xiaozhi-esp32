#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <string>
#include <memory>
#include <esp_image_format.h>

class Theme;

class Display {
public:
    virtual ~Display() = default;

    virtual bool SetStatus(const char* status) = 0;
    virtual bool ShowNotification(const char* notification, int duration_ms = 3000) = 0;
    virtual bool ShowNotification(const std::string& notification, int duration_ms = 3000) = 0;
    virtual bool SetPreviewImage(const esp_image_t* image) = 0;
    virtual bool SetPreviewImage(const uint8_t* data, size_t size) = 0;
    virtual bool SetStatusMode(bool mode) = 0;
    virtual bool SnapshotToJpeg(std::string& jpeg_data, int quality = 80) = 0;
    virtual bool SetChatMessage(const char* role, const char* content) = 0;
    virtual bool ClearChatMessage() = 0;
    virtual bool SetEmotion(const char* emotion) = 0;
    virtual bool SetTheme(const char* theme) = 0;
    virtual bool SetTheme(std::shared_ptr<const Theme> theme) = 0;
    virtual bool SupportsDynamicGlyph() { return false; }
    virtual void SetPowerSaveMode(bool on) = 0;
    virtual void UpdateStatusbar(bool update_all = false);

protected:
    virtual bool Lock(int timeout_ms = 0) = 0;
    virtual void Unlock() = 0;
};

#endif // _DISPLAY_H_
