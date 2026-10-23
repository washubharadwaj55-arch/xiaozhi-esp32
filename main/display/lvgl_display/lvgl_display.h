#ifndef LVGL_DISPLAY_H
#define LVGL_DISPLAY_H

#include "display.h"

#include <string>
#include <chrono>
#include <memory>
#include <functional>

#include <esp_log.h>
#include <esp_err.h>
#include <esp_timer.h>

#include <lvgl.h>

#include "dynamic_glyph_cache.h"
#include "lvgl_font.h"
#include "lvgl_theme.h"

class LvglImage;
class LvglThemeManager;

class LvglDisplay : public Display {
public:
    LvglDisplay();
    ~LvglDisplay();

    virtual bool SetStatus(const char* status) override;
    virtual bool ShowNotification(const char* notification, int duration_ms = 3000) override;
    virtual bool ShowNotification(const std::string& notification, int duration_ms = 3000) override;
    virtual bool SetPreviewImage(const esp_image_t* image) override;
    virtual bool SetPreviewImage(const uint8_t* data, size_t size) override;
    virtual bool SetStatusMode(bool mode) override;
    virtual bool SnapshotToJpeg(std::string jpeg_data, int quality = 80) override;
    virtual bool SetChatMessage(const char* role, const char* content) override;
    virtual bool ClearChatMessage() override;
    virtual bool SetEmotion(const char* emotion) override;
    virtual bool SetTheme(const char* theme) override;
    virtual bool SetTheme(std::shared_ptr<const Theme> theme) override;
    virtual bool SupportsDynamicGlyph() override { return true; }

    void SetThemeManager(std::shared_ptr<LvglThemeManager> theme_manager);
    void SetTheme(LvglTheme* theme);
    void Tick();

    void SetChatMessage(std::function<void(const char*, const char*)> callback);
    void SetEmotionMessage(std::function<void(const char*)> callback);

protected:
    esp_pm_lock_handle_t pm_lock_ = nullptr;
    lv_display_t* lv_display_ = nullptr;

    lv_obj_t* network_label_ = nullptr;
    lv_obj_t* status_label_ = nullptr;
    lv_obj_t* notification_label_ = nullptr;
    lv_obj_t* mute_label_ = nullptr;
    lv_obj_t* chat_message_label_ = nullptr;
    lv_obj_t* emotion_label_ = nullptr;
    lv_obj_t* battery_label_ = nullptr;
    lv_obj_t* low_battery_popup_ = nullptr;

    char* battery_icon_ = nullptr;
    lv_timer_t* low_battery_timer_ = nullptr;
    bool muted_ = false;

    std::chrono::system_clock::time_point last_status_update_time_;
    esp_timer_handle_t notification_timer_ = nullptr;
    int last_displayed_clock_min_ = -1;
    std::unique_ptr<DynamicGlyphCache> dynamic_glyph_cache_;

    friend class DisplayNetwork;
    virtual void Lock();
    virtual void Unlock();
};

#endif // LVGL_DISPLAY_H
