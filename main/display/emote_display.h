#ifndef _EMOTE_DISPLAY_H_
#define _EMOTE_DISPLAY_H_

#include "display.h"
#include <cstdint>
#include <string>

class EmoteDisplay : public Display {
public:
    EmoteDisplay();
    virtual ~EmoteDisplay() override = default;

    virtual bool SetStatus(const char* status) override { (void)status; return true; }
    virtual bool ShowNotification(const char* notification, int duration_ms = 3000) override {
        (void)notification; (void)duration_ms; return true;
    }
    virtual bool SetEmotion(const char* emotion) override { (void)emotion; return true; }
    virtual bool SetIcon(const char* icon) override { (void)icon; return true; }

    bool Lock(const int timeout_ms = 0);
    virtual bool Lock(int timeout_ms = 0) override { return Lock((const int)timeout_ms); }
    virtual void Unlock() override;

    bool StopAnimDialog();
    bool InsertAnimDialog(const char* emoji_name, uint32_t duration_ms = 0);
    void RefreshAll();
};

#endif // _EMOTE_DISPLAY_H_

