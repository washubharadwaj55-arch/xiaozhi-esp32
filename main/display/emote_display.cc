#include "emote_display.h"
#include <esp_log.h>

static const char* TAG = "EmoteDisplay";

EmoteDisplay::EmoteDisplay() {
    ESP_LOGI(TAG, "EmoteDisplay initialized");
}

bool EmoteDisplay::Lock(int timeout_ms) {
    (void)timeout_ms;
    return true;
}

void EmoteDisplay::Unlock() {
}

bool EmoteDisplay::StopAnimDialog() {
    return true;
}

bool EmoteDisplay::InsertAnimDialog(const char* emoji_name, uint32_t duration_ms) {
    (void)emoji_name;
    (void)duration_ms;
    return true;
}

void EmoteDisplay::RefreshAll() {
}
