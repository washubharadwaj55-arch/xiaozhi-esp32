#ifndef EMOTE_DISPLAY_H
#define EMOTE_DISPLAY_H

#include "display.h"

#include <string>
#include <memory>
#include <vector>

#include <esp_err.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>

class EmoteDisplay : public Display {
public:
    EmoteDisplay(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_handle_t panel_handle, int width, int height);
    virtual ~EmoteDisplay();

    // Base class Display virtual overrides
    virtual bool SetStatus(const char* status) override;
    virtual bool ShowNotification(const char* notification, int duration_ms = 3000) override;
    virtual bool ShowNotification(const std::string& notification, int duration_ms = 3000) override;
    virtual bool SetEmotion(const char* emotion) override;
    virtual bool SetChatMessage(const char* role, const char* content) override;
    virtual bool ClearChatMessage() override;
    virtual bool SetTheme(const char* theme) override;
    virtual bool SetTheme(std::shared_ptr<const Theme> theme) override;
    virtual void SetPowerSaveMode(bool on) override;
    virtual void UpdateStatusbar(bool update_all = false);

    // Emote specific methods
    bool InsertAnimation(const char* emoji_name, uint32_t duration_ms);
    bool MountAssets(const char* partition_label);
    void UnmountAssets();
    bool GetAssetData(const std::string& name, const uint8_t*& data, size_t& size);
    void LoadAssets();

protected:
    virtual void Lock() override;
    virtual void Unlock() override;

private:
    esp_lcd_panel_io_handle_t panel_io_ = nullptr;
    esp_lcd_panel_handle_t panel_handle_ = nullptr;
};

#endif // EMOTE_DISPLAY_H
