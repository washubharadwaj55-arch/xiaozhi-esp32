#ifndef LVGL_DISPLAY_H
#define LVGL_DISPLAY_H

#include "display.h"
#include "lvgl_image.h"

#include <esp_log.h>
#include <esp_pm.h>
#include <esp_timer.h>
#include <lvgl.h>

#include <chrono>
#include <functional>
#include <memory>
#include <string>

class DynamicGlyphCache;
class LvglFont;
class LvglTheme;
class LvglThemeManager;

class LvglDisplay : public Display {
public:
    LvglDisplay();
    virtual ~LvglDisplay();

    virtual void SetStatus(const char* status) override;
    virtual void ShowNotification(const char* notification, int duration_ms = 3000) override;
    virtual void ShowNotification(const std::string& notification, int duration_ms = 3000) override;
    virtual void SetPreviewImage(std::unique_ptr<LvglImage> image) override;
    virtual void UpdateStatusBar(bool update_all = false) override;
    virtual void SetPowerSaveMode(bool on) override;
    virtual bool SnapshotToJpeg(std::string& jpeg_data, int quality = 80) override;
    virtual bool AddTextGlyphs(const std::vector<TextGlyph>& glyphs, uint8_t bpp) override;
    virtual bool ClearTextGlyphs() override;
    virtual bool SetTextFont(std::shared_ptr<LvglFont> text_font) override;
    virtual bool SupportsGuiOperations() const override { return true; }

    virtual void SetEmotion(const char* emotion) override;
    void SetThemeManager(std::shared_ptr<LvglThemeManager> theme_manager);
    void SetTheme(LvglTheme* theme);
    void Tick(uint32_t period);
    void SetChatMessage(std::function<void(const char*, const char*)> callback);
    void SetEmotionMessage(std::function<void(const char*)> callback);

protected:
    esp_pm_lock_handle_t pm_lock = nullptr;
    lv_display_t* display = nullptr;

    lv_obj_t* network_label = nullptr;
    lv_obj_t* status_label = nullptr;
    lv_obj_t* notification_label = nullptr;
    lv_obj_t* mute_label = nullptr;
    lv_obj_t* battery_label = nullptr;
    lv_obj_t* low_battery_popup = nullptr;
    lv_obj_t* low_battery_label = nullptr;

    char* battery_icon = nullptr;
    char* network_icon = nullptr;
    bool muted = false;

    std::chrono::system_clock::time_point last_status_update_time;
    esp_timer_handle_t notification_timer = nullptr;
    int last_displayed_clock_min = -1; // -1 forces update on first idle tick
    std::unique_ptr<DynamicGlyphCache> dynamic_glyph_cache;

    friend class DisplayLockGuard;
    virtual void lock(int timeout_ms = 0) = 0;
    virtual void unlock() = 0;
};

#endif
