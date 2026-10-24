#ifndef _LVGL_DISPLAY_H
#define _LVGL_DISPLAY_H

#include <vector>
#include "display.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <esp_timer.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>
#include <esp_pm.h>

#include <string>
#include <functional>
#include <memory>
#include <map>
#include <chrono>

#include <lvgl.h>

#include "dynamic_glyph_cache.h"
#include "lvgl_font.h"
#include "lvgl_theme.h"

#ifndef MATERIAL_SYMBOLS_BATTERY_CHARGING_FULL
#define MATERIAL_SYMBOLS_BATTERY_CHARGING_FULL "\xEE\x86\xA5"
#endif
#ifndef MATERIAL_SYMBOLS_BATTERY_FULL
#define MATERIAL_SYMBOLS_BATTERY_FULL "\xEE\x86\xA4"
#endif
#ifndef MATERIAL_SYMBOLS_BATTERY_6_BAR
#define MATERIAL_SYMBOLS_BATTERY_6_BAR "\xEE\x86\xA4"
#endif
#ifndef MATERIAL_SYMBOLS_BATTERY_5_BAR
#define MATERIAL_SYMBOLS_BATTERY_5_BAR "\xEE\x86\xA4"
#endif
#ifndef MATERIAL_SYMBOLS_BATTERY_4_BAR
#define MATERIAL_SYMBOLS_BATTERY_4_BAR "\xEE\x86\xA4"
#endif
#ifndef MATERIAL_SYMBOLS_BATTERY_3_BAR
#define MATERIAL_SYMBOLS_BATTERY_3_BAR "\xEE\x86\xA3"
#endif
#ifndef MATERIAL_SYMBOLS_BATTERY_2_BAR
#define MATERIAL_SYMBOLS_BATTERY_2_BAR "\xEE\x86\xA2"
#endif
#ifndef MATERIAL_SYMBOLS_BATTERY_1_BAR
#define MATERIAL_SYMBOLS_BATTERY_1_BAR "\xEE\x86\xA1"
#endif
#ifndef MATERIAL_SYMBOLS_BATTERY_0_BAR
#define MATERIAL_SYMBOLS_BATTERY_0_BAR "\xEE\x86\xA0"
#endif

class LvglImage;
class LvglThemeManager;

class DisplayLockGuard {
public:
    DisplayLockGuard(Display* display) : display_(display) {
        if (display_) display_->Lock();
    }
    ~DisplayLockGuard() {
        if (display_) display_->Unlock();
    }
private:
    Display* display_;
};

class LvglDisplay : public Display {
public:
    LvglDisplay();
    virtual ~LvglDisplay();

    virtual bool ShowNotification(const char* notification, int duration_ms = 3000) override;
    virtual bool ShowNotification(const std::string& notification, int duration_ms = 3000) override;

    virtual bool SetStatus(const char* status) override;
    virtual void SetStatus(const std::string& status);

    virtual bool SetEmotion(const char* emotion) override;
    virtual void SetEmotion(const std::string& emotion);

    virtual bool SetIcon(const char* icon) override { (void)icon; return true; }

    virtual void SetChatMessage(const char* role, const char* content) override;
    virtual void ClearChatMessage();
    virtual void ClearChatMessages() override;

    virtual bool SetTheme(const char* theme) override;
    virtual bool SetTheme(std::shared_ptr<const Theme> theme) override;
    virtual void SetTheme(Theme* theme) override { (void)theme; }
    virtual void SetTheme(LvglTheme* theme) override;
    void SetTheme(const std::string& theme_name);
    void SetTheme(std::shared_ptr<Theme> theme);

    virtual bool SetImage(const uint8_t* data, size_t size) override { (void)data; (void)size; return true; }
    virtual bool SnapshotToJpeg(std::string path, int quality) override { (void)path; (void)quality; return true; }

    virtual bool SnapshotJpeg(std::string& jpeg_data, int quality = 80);
    virtual bool SnapshotPng(std::string& png_data, size_t size);
    virtual void SetPreviewImage(const uint8_t* data, size_t size);
    virtual void SetPreviewImage(std::unique_ptr<LvglImage> image) { (void)image; }
    virtual bool SupportsDynamicGlyph() { return true; }

    std::shared_ptr<LvglThemeManager> theme_manager() const { return theme_manager_; }

    virtual bool Lock(int timeout_ms = 0) override;
    virtual void Unlock() override;

    void SetDirectionGetMethod(const char* method, std::function<void(const char*)> callback);
    void SetEmotionSelectedMethod(std::function<void(const char*)> callback);

    virtual void UpdateStatusbar(bool update_all = false) override;
    virtual void SetupUI() override;

protected:
    virtual bool Lock() = 0;

    esp_lcd_panel_io_handle_t panel_io_ = nullptr;
    esp_lcd_panel_handle_t panel_ = nullptr;

    lv_display_t* display_ = nullptr;
    lv_obj_t* status_bar_ = nullptr;
    lv_obj_t* status_label_ = nullptr;
    lv_obj_t* content_ = nullptr;
    lv_obj_t* container_ = nullptr;
    lv_obj_t* emotion_label_ = nullptr;
    lv_obj_t* network_label_ = nullptr;
    lv_obj_t* mute_label_ = nullptr;
    lv_obj_t* message_label_ = nullptr;
    lv_obj_t* chat_message_label_ = nullptr;
    lv_obj_t* battery_label_ = nullptr;
    lv_obj_t* battery_progress_ = nullptr;
    lv_obj_t* notification_label_ = nullptr;
    lv_obj_t* low_battery_popup_ = nullptr;
    lv_obj_t* low_battery_label_ = nullptr;
    lv_obj_t* preview_image_ = nullptr;

    int width_ = 240;
    int height_ = 240;
    bool setup_ui_called_ = false;

    esp_timer_handle_t notification_timer_ = nullptr;
    esp_pm_lock_handle_t pm_lock_ = nullptr;

    bool muted_ = false;
    int battery_icon_ = 0;
    int battery_level_ = -1;

    std::shared_ptr<LvglThemeManager> theme_manager_;
    std::string current_theme_name_;
    LvglTheme* current_theme_ = nullptr;

    std::chrono::system_clock::time_point last_status_update_time_;
    esp_timer_handle_t status_bar_timer_ = nullptr;
    bool show_status_bar_ = false;

    std::unique_ptr<DynamicGlyphCache> dynamic_glyph_cache_;

    friend class DisplayNetwork;
    virtual void Tick();
};

#endif // _LVGL_DISPLAY_H
