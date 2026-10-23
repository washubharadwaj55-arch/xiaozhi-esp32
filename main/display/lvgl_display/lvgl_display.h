#ifndef _LVGL_DISPLAY_H
#define _LVGL_DISPLAY_H

#include "display.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <esp_timer.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>

#include <string>
#include <functional>
#include <memory>
#include <map>
#include <chrono>

#include <lvgl.h>
#include <font_awesome.h>

#include "dynamic_glyph_cache.h"
#include "lvgl_font.h"
#include "lvgl_theme.h"

class LvglImage;
class LvglThemeManager;

class LvglDisplay : public Display {
public:
    LvglDisplay();
    virtual ~LvglDisplay();

    virtual void ShowNotification(const std::string& notification, int duration_ms = 3000) override;
    virtual void ShowNotification(const char* notification, int duration_ms = 3000);
    virtual void SetStatus(const std::string& status) override;
    virtual void SetStatus(const char* status);
    virtual void SetEmotion(const std::string& emotion) override;
    virtual void SetEmotion(const char* emotion);
    virtual void SetChatMessage(const char* role, const char* content) override;
    virtual void ClearChatMessage() override;
    virtual bool SnapshotJpeg(std::string& jpeg_data, int quality = 80);
    virtual bool SnapshotPng(std::string& png_data, size_t size);
    virtual void SetPreviewImage(const uint8_t* data, size_t size);
    virtual bool SupportsDynamicGlyph() { return true; }

    std::shared_ptr<LvglThemeManager> theme_manager() const { return theme_manager_; }
    void SetTheme(const std::string& theme_name);
    void SetTheme(std::shared_ptr<Theme> theme);

    virtual bool Lock(int timeout_ms = 0) override;
    virtual void Unlock() override;

    void SetDirectionGetMethod(const char* method, std::function<void(const char*)> callback);
    void SetEmotionSelectedMethod(std::function<void(const char*)> callback);

protected:
    virtual bool Lock() = 0;

    esp_lcd_panel_io_handle_t panel_io_ = nullptr;
    esp_lcd_panel_handle_t panel_ = nullptr;

    lv_display_t* display_ = nullptr;
    lv_obj_t* status_bar_ = nullptr;
    lv_obj_t* content_ = nullptr;
    lv_obj_t* container_ = nullptr;
    lv_obj_t* emotion_label_ = nullptr;
    lv_obj_t* network_label_ = nullptr;
    lv_obj_t* mute_label_ = nullptr;
    lv_obj_t* message_label_ = nullptr;
    lv_obj_t* chat_message_label_ = nullptr;
    lv_obj_t* battery_label_ = nullptr;
    lv_obj_t* battery_progress_ = nullptr;

    bool muted_ = false;
    int battery_icon_ = 0;
    int battery_level_ = -1;

    std::shared_ptr<LvglThemeManager> theme_manager_;
    std::string current_theme_name_;

    std::chrono::system_clock::time_point last_status_update_time_;
    esp_timer_handle_t status_bar_timer_ = nullptr;
    bool show_status_bar_ = false;

    std::unique_ptr<DynamicGlyphCache> glyph_cache_;

    void UpdateStatusbar(bool force = false);

    friend class DisplayNetwork;
    virtual void SetupUI();
    virtual void Tick();
};

#endif // _LVGL_DISPLAY_H
