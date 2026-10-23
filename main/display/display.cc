#include "display.h"

#include <esp_log.h>

static const char* TAG = "Display";

bool Display::SetStatus(const char* status) {
    ESP_LOGI(TAG, "SetStatus: %s", status ? status : "(null)");
    return true;
}

bool Display::ShowNotification(const char* notification, int duration_ms) {
    ESP_LOGI(TAG, "ShowNotification: %s (%d ms)", notification ? notification : "(null)", duration_ms);
    return true;
}

bool Display::SetEmotion(const char* emotion) {
    ESP_LOGI(TAG, "SetEmotion: %s", emotion ? emotion : "(null)");
    return true;
}

bool Display::SetIcon(const char* icon) {
    ESP_LOGI(TAG, "SetIcon: %s", icon ? icon : "(null)");
    return true;
}

bool Display::SetChatMessage(const char* role, const char* content) {
    ESP_LOGI(TAG, "SetChatMessage [%s]: %s", role ? role : "(null)", content ? content : "(null)");
    return true;
}

bool Display::ClearChatMessages() {
    ESP_LOGI(TAG, "ClearChatMessages");
    return true;
}

bool Display::SetTheme(const char* theme) {
    ESP_LOGI(TAG, "SetTheme: %s", theme ? theme : "(null)");
    return true;
}

bool Display::SetTheme(std::shared_ptr<const Theme> theme) {
    ESP_LOGI(TAG, "SetTheme (object): %s", theme ? "set" : "null");
    return theme != nullptr;
}

bool Display::SetImage(const uint8_t* data, size_t size) {
    ESP_LOGI(TAG, "SetImage (raw): %u bytes", (unsigned int)size);
    return data != nullptr && size > 0;
}

bool Display::SnapshotToJpeg(std::string path, int quality) {
    (void)quality;
    ESP_LOGW(TAG, "SnapshotToJpeg not supported by base Display: %s", path.c_str());
    return false;
}

void Display::UpdateStatusbar(bool update_all) {
    (void)update_all;
}

bool Display::Lock(int timeout_ms) {
    (void)timeout_ms;
    return true;
}

void Display::Unlock() {
}
