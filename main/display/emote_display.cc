#include "emote_display.h"
#include <esp_log.h>

#define TAG "EmoteDisplay"

namespace emote {

// Methods implementation for EmoteDisplay
bool EmoteDisplay::Lock(const int timeout_ms) {
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

} // namespace emote
