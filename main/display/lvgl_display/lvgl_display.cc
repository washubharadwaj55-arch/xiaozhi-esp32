#include "lvgl_display.h"

#include <esp_err.h>
#include <esp_log.h>
#include <material_symbols.h>
#include <cstdlib>
#include <cstring>
#include <string>

#include "application.h"
#include "assets/lang_config.h"
#include "audio_codec.h"
#include "board.h"
#include "dynamic_glyph_cache.h"
#include "jpg/image_to_jpeg.h"
#include "lvgl_display.h"
#include "lvgl_font.h"
#include "lvgl_theme.h"
#include "settings.h"

#define TAG "LvglDisplay"

LvglDisplay::LvglDisplay() {
    dynamic_glyph_cache_ = std::make_unique<DynamicGlyphCache>();

    // Notification timer
    esp_timer_create_args_t notification_timer_args = {
        .callback = [](void* arg) {
            LvglDisplay* display = static_cast<LvglDisplay*>(arg);
            DisplayLockGuard lock(display);
            if (display->notification_label_ != nullptr) {
                lv_obj_add_flag(display->notification_label_, LV_OBJ_FLAG_HIDDEN);
            }
        },
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "notification_timer",
        .skip_unhandled_events = false,
    };
    ESP_ERROR_CHECK(esp_timer_create(&notification_timer_args, &notification_timer_));

    // Create a power management lock
    auto ret = esp_pm_lock_create(ESP_PM_LOCK_CPU_FREQ_MAX, 0, "display_update", &pm_lock_);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Power management lock created");
    } else {
        ESP_LOGW(TAG, "Power management lock not supported: %d", ret);
    }
}

LvglDisplay::~LvglDisplay() {
    if (notification_timer_ != nullptr) {
        esp_timer_stop(notification_timer_);
        esp_timer_delete(notification_timer_);
    }
    if (pm_lock_ != nullptr) {
        esp_pm_lock_delete(pm_lock_);
    }
    if (display_ != nullptr) {
        lv_display_delete(display_);
    }
}

void LvglDisplay::SetTheme(const std::string& theme_name) {
    if (theme_manager_ == nullptr) {
        return;
    }
    current_theme_name_ = theme_name;
    auto theme = theme_manager_->GetTheme(theme_name);
    if (theme != nullptr) {
        SetTheme(theme);
    }
}

void LvglDisplay::SetTheme(std::shared_ptr<Theme> theme) {
    // Override in derived class if required
}

void LvglDisplay::UpdateStatusbar(bool force) {
    if (status_bar_ == nullptr) {
        return;
    }

    auto now = std::chrono::system_clock::now();
    if (!force && now - last_status_update_time_ < std::chrono::seconds(1)) {
        return;
    }
    last_status_update_time_ = now;

    DisplayLockGuard lock(this);

    // Update battery status
    auto& board = Board::GetInstance();
    int battery_level = 0;
    bool charging = false;
    bool discharging = false;
    if (board.GetBatteryLevel(battery_level, charging, discharging)) {
        if (battery_label_ != nullptr) {
            lv_obj_clear_flag(battery_label_, LV_OBJ_FLAG_HIDDEN);
            if (charging) {
                lv_label_set_text(battery_label_, MATERIAL_SYMBOLS_BATTERY_CHARGING_FULL);
            } else {
                const char* icon = MATERIAL_SYMBOLS_BATTERY_0_BAR;
                if (battery_level > 90) icon = MATERIAL_SYMBOLS_BATTERY_FULL;
                else if (battery_level > 80) icon = MATERIAL_SYMBOLS_BATTERY_6_BAR;
                else if (battery_level > 60) icon = MATERIAL_SYMBOLS_BATTERY_5_BAR;
                else if (battery_level > 50) icon = MATERIAL_SYMBOLS_BATTERY_4_BAR;
                else if (battery_level > 30) icon = MATERIAL_SYMBOLS_BATTERY_3_BAR;
                else if (battery_level > 15) icon = MATERIAL_SYMBOLS_BATTERY_2_BAR;
                else if (battery_level > 5) icon = MATERIAL_SYMBOLS_BATTERY_1_BAR;
                lv_label_set_text(battery_label_, icon);
            }
        }
    }
}

void LvglDisplay::ShowNotification(const std::string& notification, int duration_ms) {
    ShowNotification(notification.c_str(), duration_ms);
}

void LvglDisplay::ShowNotification(const char* notification, int duration_ms) {
    DisplayLockGuard lock(this);
    if (notification_label_ == nullptr) {
        return;
    }
    lv_label_set_text(notification_label_, notification);
    lv_obj_clear_flag(notification_label_, LV_OBJ_FLAG_HIDDEN);

    if (duration_ms > 0) {
        esp_timer_stop(notification_timer_);
        esp_timer_start_once(notification_timer_, duration_ms * 1000);
    }
}

void LvglDisplay::SetStatus(const std::string& status) {
    SetStatus(status.c_str());
}

void LvglDisplay::SetStatus(const char* status) {
    DisplayLockGuard lock(this);
    if (message_label_ == nullptr) {
        return;
    }
    lv_label_set_text(message_label_, status);
}

void LvglDisplay::SetEmotion(const std::string& emotion) {
    SetEmotion(emotion.c_str());
}

void LvglDisplay::SetEmotion(const char* emotion) {
    DisplayLockGuard lock(this);
    if (emotion_label_ == nullptr) {
        return;
    }
    lv_label_set_text(emotion_label_, emotion);
}

void LvglDisplay::SetChatMessage(const char* role, const char* content) {
    DisplayLockGuard lock(this);
    if (chat_message_label_ == nullptr) {
        return;
    }
    lv_label_set_text(chat_message_label_, content);
}

void LvglDisplay::ClearChatMessage() {
    DisplayLockGuard lock(this);
    if (chat_message_label_ == nullptr) {
        return;
    }
    lv_label_set_text(chat_message_label_, "");
}

bool LvglDisplay::SnapshotJpeg(std::string& jpeg_data, int quality) {
    DisplayLockGuard lock(this);
    if (display_ == nullptr) {
        return false;
    }
    return false;
}

bool LvglDisplay::SnapshotPng(std::string& png_data, size_t size) {
    DisplayLockGuard lock(this);
    if (display_ == nullptr) {
        return false;
    }
    return false;
}

void LvglDisplay::SetPreviewImage(const uint8_t* data, size_t size) {
    // Preview implementation
}

bool LvglDisplay::Lock(int timeout_ms) {
    return Lock();
}

void LvglDisplay::Unlock() {
    // Unlock implementation
}

void LvglDisplay::SetDirectionGetMethod(const char* method, std::function<void(const char*)> callback) {
    // Direction callback registration
}

void LvglDisplay::SetEmotionSelectedMethod(std::function<void(const char*)> callback) {
    // Emotion selected callback registration
}

void LvglDisplay::SetupUI() {
    // Base UI setup
}

void LvglDisplay::Tick() {
    UpdateStatusbar(false);
}
