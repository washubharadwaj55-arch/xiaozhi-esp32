#ifndef _OLED_DISPLAY_H_
#define _OLED_DISPLAY_H_

#include "lvgl_display/lvgl_display.h"
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>

class OledDisplay : public LvglDisplay {
public:
    OledDisplay(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_handle_t panel,
                int width, int height, bool mirror_x, bool mirror_y);
    virtual ~OledDisplay() override;

    virtual bool Lock() override { return LvglDisplay::Lock(); }
    virtual bool Lock(int timeout_ms) override { return LvglDisplay::Lock(timeout_ms); }
    virtual void Unlock() override { LvglDisplay::Unlock(); }

    virtual void SetupUI() override;
    virtual bool SetEmotion(const char* emotion) override;
    virtual void SetTheme(Theme* theme) override;
    virtual void SetPowerSaveMode(bool on) override;

private:
    void SetupUI_128x64();
    void SetupUI_128x32();

    lv_obj_t* side_bar_ = nullptr;
};

#endif // _OLED_DISPLAY_H_
