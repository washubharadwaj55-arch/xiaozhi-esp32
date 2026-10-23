#include "lvgl_display.h"
#include <esp_log.h>
#include <esp_err.h>

static const char* TAG = "LvglDisplay";

LvglDisplay::LvglDisplay() {
    dynamic_glyph_cache_ = std::make_unique<DynamicGlyphCache>();
    
    #if CONFIG_PM_ENABLE
    esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "display_update", &pm_lock_);
    #endif
}

LvglDisplay::~LvglDisplay() {
    #if CONFIG_PM_ENABLE
    if (pm_lock_ != nullptr) {
        esp_pm_lock_delete(pm_lock_);
    }
    #endif
}

bool LvglDisplay::ShowNotification(const char* notification, int duration_ms) {
    ESP_LOGI(TAG, "ShowNotification: %s (%d ms)", notification ? notification : "(null)", duration_ms);
    return true;
}

bool LvglDisplay::ShowNotification(const std::string& notification, int duration_ms) {
    return ShowNotification(notification.c_str(), duration_ms);
}

bool LvglDisplay::SetStatus(const char* status) {
    ESP_LOGI(TAG, "SetStatus: %s", status ? status : "(null)");
    return true;
}

void LvglDisplay::SetStatus(const std::string& status) {
    SetStatus(status.c_str());
}

bool LvglDisplay::SetEmotion(const char* emotion) {
    ESP_LOGI(TAG, "SetEmotion: %s", emotion ? emotion : "(null)");
    return true;
}

void LvglDisplay::SetEmotion(const std::string& emotion) {
    SetEmotion(emotion.c_str());
}

bool LvglDisplay::SetChatMessage(const char* role, const char* content) {
    ESP_LOGI(TAG, "SetChatMessage [%s]: %s", role ? role : "(null)", content ? content : "(null)");
    return true;
}

void LvglDisplay::ClearChatMessage() {
    ESP_LOGI(TAG, "ClearChatMessage");
}

bool LvglDisplay::ClearChatMessages() {
    ClearChatMessage();
    return true;
}

void LvglDisplay::SetTheme(const std::string& theme_name) {
    current_theme_name_ = theme_name;
}

void LvglDisplay::SetTheme(std::shared_ptr<Theme> theme) {
    (void)theme;
}

void LvglDisplay::SetTheme(LvglTheme* theme) {
    (void)theme;
}

bool LvglDisplay::SetTheme(const char* theme) {
    if (theme) {
        SetTheme(std::string(theme));
    }
    return true;
}

bool LvglDisplay::SetTheme(std::shared_ptr<const Theme> theme) {
    (void)theme;
    return true;
}

bool LvglDisplay::SnapshotJpeg(std::string& jpeg_data, int quality) {
    (void)jpeg_data;
    (void)quality;
    return false;
}

bool LvglDisplay::SnapshotPng(std::string& png_data, size_t size) {
    (void)png_data;
    (void)size;
    return false;
}

void LvglDisplay::SetPreviewImage(const uint8_t* data, size_t size) {
    (void)data;
    (void)size;
}

bool LvglDisplay::Lock(int timeout_ms) {
    (void)timeout_ms;
    return true;
}

void LvglDisplay::Unlock() {
}

void LvglDisplay::SetDirectionGetMethod(const char* method, std::function<void(const char*)> callback) {
    (void)method;
    (void)callback;
}

void LvglDisplay::SetEmotionSelectedMethod(std::function<void(const char*)> callback) {
    (void)callback;
}

void LvglDisplay::UpdateStatusbar(bool update_all) {
    (void)update_all;
}

void LvglDisplay::SetupUI() {
}

void LvglDisplay::Tick() {
}
