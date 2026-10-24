#include "oled_display.h"
#include "assets/lang_config.h"
#include "lvgl_theme.h"
#include "settings.h"

#include <algorithm>
#include <string>
#include <cstring>
#include <driver/gpio.h>

#include <esp_err.h>
#include <esp_log.h>
#include <esp_lvgl_port.h>
#include <lvgl.h>
#include <material_symbols.h>
#include <noto_emoji.h>

#define TAG "OledDisplay"

LV_FONT_DECLARE(BUILTIN_TEXT_FONT);
LV_FONT_DECLARE(BUILTIN_ICON_FONT);
LV_FONT_DECLARE(font_material_symbols_30_1);
LV_FONT_DECLARE(font_noto_emoji_30_1);

OledDisplay::OledDisplay(esp_lcd_panel_io_handle_t io_handle, esp_lcd_panel_handle_t io_panel,
                         int width, int height, bool mirror_x, bool mirror_y)
    : panel_io_(io_handle), io_panel_(io_panel),
      width_(width), height_(height) {

    auto text_font = std::make_shared<LvglFont>(&BUILTIN_TEXT_FONT);
    auto icon_font = std::make_shared<LvglFont>(&BUILTIN_ICON_FONT);
    auto large_icon_font = std::make_shared<LvglFont>(&font_material_symbols_30_1);
    auto emoji_font = std::make_shared<LvglFont>(&font_noto_emoji_30_1);

    auto dark_theme = std::make_shared<LvglTheme>("dark");
    dark_theme->set_text_font(text_font);
    dark_theme->set_icon_font(icon_font);
    dark_theme->set_large_icon_font(large_icon_font);
    dark_theme->set_emoji_font(emoji_font);

    auto& theme_manager = LvglThemeManager::GetInstance();
    theme_manager.AddTheme("dark", dark_theme);
    current_theme_ = dark_theme;

    ESP_LOGI(TAG, "Initialize LVGL");
    lvgl_port_cfg_t lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_cfg.task_priority = 4;
    lvgl_port_cfg.task_stack = 6144;
#if CONFIG_SOC_CPU_CORES_NUM > 1
    lvgl_port_cfg.task_affinity = 1;
#endif
    ESP_ERROR_CHECK(lvgl_port_init(&lvgl_port_cfg));

    ESP_LOGI(TAG, "Adding OLED display");
    const lvgl_port_display_cfg_t display_cfg = {
        .io_handle = panel_io_,
        .panel_handle = io_panel_,
        .control_handle = nullptr,
        .buffer_size = static_cast<uint32_t>(width * height),
        .double_buffer = false,
        .hres = static_cast<uint32_t>(width),
        .vres = static_cast<uint32_t>(height),
        .monochrome = true,
        .rotation =
            {
                .swap_xy = false,
                .mirror_x = mirror_x,
                .mirror_y = mirror_y,
            },
        .flags =
            {
                .buff_dma = 1,
                .buff_spiram = 0,
                .sw_rotate = 0,
                .full_refresh = 0,
                .direct_mode = 0,
            },
    };

    display_ = lvgl_port_add_disp(&display_cfg);
    if (display_ == nullptr) {
        ESP_LOGE(TAG, "Failed to add display");
        return;
    }
}

OledDisplay::~OledDisplay() {
    DisplayLockGuard lock(this);
    if (content_ != nullptr) {
        lv_obj_del(content_);
    }
    if (status_bar_ != nullptr) {
        lv_obj_del(status_bar_);
    }
    if (side_bar_ != nullptr) {
        lv_obj_del(side_bar_);
    }
    if (container_ != nullptr) {
        lv_obj_del(container_);
    }
    if (panel_io_ != nullptr) {
        esp_lcd_panel_io_del(panel_io_);
    }
    if (io_panel_ != nullptr) {
        esp_lcd_panel_del(io_panel_);
    }
    lvgl_port_deinit();
}

void OledDisplay::SetupUI() {
    if (setup_ui_called_) {
        ESP_LOGW(TAG, "SetupUI called multiple times, skipping duplicate call");
        return;
    }

    Display::SetupUI();
    if (height_ == 64) {
        SetupUI_128x64();
    } else {
        SetupUI_128x32();
    }
}

void OledDisplay::SetupUI_128x64() {
    DisplayLockGuard lock(this);
    if (!lock || current_theme_ == nullptr) {
        return;
    }

    auto text_font = current_theme_->GetTextFont();
    auto icon_font = current_theme_->GetIconFont();
    auto large_icon_font = current_theme_->GetLargeIconFont();

    lv_obj_t* screen = lv_screen_active();
    lv_obj_clean(screen);
    lv_obj_set_style_text_font(screen, text_font.get(), 0);

    container_ = lv_obj_create(screen);
    lv_obj_set_size(container_, width_, height_);
    lv_obj_clear_flag(container_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_style_all(container_);
    lv_obj_set_flex_flow(container_, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(container_, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    content_ = lv_obj_create(container_);
    lv_obj_set_size(content_, width_ - 12, height_ - 16);
    lv_obj_clear_flag(content_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_style_all(content_);
    lv_obj_set_flex_flow(content_, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(content_, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    emotion_label_ = lv_label_create(content_);
    lv_obj_set_style_text_font(emotion_label_, large_icon_font.get(), 0);
    lv_label_set_text(emotion_label_, MATERIAL_SYMBOLS_ROBOT_2);
    lv_obj_set_style_text_align(emotion_label_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_top(emotion_label_, 10, 0);
    lv_obj_add_flag(emotion_label_, LV_OBJ_FLAG_HIDDEN);

    chat_message_label_ = lv_label_create(content_);
    lv_obj_set_size(chat_message_label_, width_ - 32, LV_SIZE_CONTENT);
    lv_obj_set_style_text_font(chat_message_label_, text_font.get(), 0);
    lv_obj_set_style_pad_top(chat_message_label_, 14, 0);
    lv_obj_set_style_text_align(chat_message_label_, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(chat_message_label_, LV_LABEL_LONG_WRAP);
    lv_label_set_text(chat_message_label_, "");

    status_bar_ = lv_obj_create(container_);
    lv_obj_set_size(status_bar_, width_ - 12, 16);
    lv_obj_remove_style_all(status_bar_);
    lv_obj_set_flex_flow(status_bar_, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(status_bar_, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    network_label_ = lv_label_create(status_bar_);
    lv_obj_set_style_text_font(network_label_, icon_font.get(), 0);
    lv_label_set_text(network_label_, "");

    status_label_ = lv_label_create(status_bar_);
    lv_obj_set_style_text_align(status_label_, LV_TEXT_ALIGN_CENTER, 0);    
    lv_obj_set_style_text_font(status_label_, text_font.get(), 0);
    lv_label_set_text(status_label_, "Washu Bhardwaj");

    notification_label_ = lv_label_create(status_bar_);
    lv_obj_set_style_text_font(notification_label_, text_font.get(), 0);
    lv_label_set_text(notification_label_, "");
    lv_obj_add_flag(notification_label_, LV_OBJ_FLAG_HIDDEN);

    mute_label_ = lv_label_create(status_bar_);
    lv_obj_set_style_text_font(mute_label_, icon_font.get(), 0);
    lv_label_set_text(mute_label_, "");

    battery_label_ = lv_label_create(status_bar_);
    lv_obj_set_style_text_font(battery_label_, icon_font.get(), 0);
    lv_label_set_text(battery_label_, "");
}

void OledDisplay::SetupUI_128x32() {
    DisplayLockGuard lock(this);
    if (!lock || current_theme_ == nullptr) {
        return;
    }

    auto text_font = current_theme_->GetTextFont();
    auto icon_font = current_theme_->GetIconFont();
    auto large_icon_font = current_theme_->GetLargeIconFont();

    lv_obj_t* screen = lv_screen_active();
    lv_obj_clean(screen);
    lv_obj_set_style_text_font(screen, text_font.get(), 0);

    container_ = lv_obj_create(screen);
    lv_obj_set_size(container_, width_, height_);
    lv_obj_clear_flag(container_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_style_all(container_);
    lv_obj_set_flex_flow(container_, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(container_, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_left(container_, 6, 0);
    lv_obj_set_style_pad_right(container_, 6, 0);

    content_ = lv_obj_create(container_);
    lv_obj_set_size(content_, width_ - 24, height_ - 8);
    lv_obj_clear_flag(content_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_style_all(content_);
    lv_obj_set_flex_flow(content_, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(content_, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    emotion_label_ = lv_label_create(content_);
    lv_obj_set_style_text_font(emotion_label_, large_icon_font.get(), 0);
    lv_label_set_text(emotion_label_, MATERIAL_SYMBOLS_ROBOT_2);
    lv_obj_add_flag(emotion_label_, LV_OBJ_FLAG_HIDDEN);

    chat_message_label_ = lv_label_create(content_);
    lv_obj_set_size(chat_message_label_, width_ - 48, LV_SIZE_CONTENT);
    lv_obj_set_style_text_font(chat_message_label_, text_font.get(), 0);
    lv_obj_set_style_pad_left(chat_message_label_, 10, 0);
    lv_label_set_long_mode(chat_message_label_, LV_LABEL_LONG_WRAP);
    lv_label_set_text(chat_message_label_, "");

    side_bar_ = lv_obj_create(container_);
    lv_obj_set_size(side_bar_, 16, height_ - 8);
    lv_obj_remove_style_all(side_bar_);
    lv_obj_set_flex_flow(side_bar_, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(side_bar_, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    network_label_ = lv_label_create(side_bar_);
    lv_obj_set_style_text_font(network_label_, icon_font.get(), 0);
    lv_label_set_text(network_label_, "");

    status_label_ = lv_label_create(side_bar_);
    lv_obj_set_style_text_font(status_label_, text_font.get(), 0);
    lv_label_set_text(status_label_, "Washu Bhardwaj");
    lv_obj_add_flag(status_label_, LV_OBJ_FLAG_HIDDEN);

    notification_label_ = lv_label_create(side_bar_);
    lv_obj_set_style_text_font(notification_label_, text_font.get(), 0);
    lv_label_set_text(notification_label_, "");
    lv_obj_add_flag(notification_label_, LV_OBJ_FLAG_HIDDEN);

    mute_label_ = lv_label_create(side_bar_);
    lv_obj_set_style_text_font(mute_label_, icon_font.get(), 0);
    lv_label_set_text(mute_label_, "");

    battery_label_ = lv_label_create(side_bar_);
    lv_obj_set_style_text_font(battery_label_, icon_font.get(), 0);
    lv_label_set_text(battery_label_, "");
}

bool OledDisplay::SetEmotion(const char* emotion) {
    if (!setup_ui_called_) {
        ESP_LOGW(TAG, "SetEmotion('%s') called before SetupUI()", emotion);
        return false;
    }

    if (emotion_label_ == nullptr) {
        return false;
    }

    auto& theme_manager = LvglThemeManager::GetInstance();
    auto light_theme = theme_manager.GetTheme("light");
    auto dark_theme = theme_manager.GetTheme("dark");
    std::shared_ptr<LvglTheme> current_theme;
    if (light_theme && lv_obj_get_screen(emotion_label_) == lv_obj_get_screen(light_theme->GetScreen())) {
        current_theme = light_theme;
    } else if (dark_theme && lv_obj_get_screen(emotion_label_) == lv_obj_get_screen(dark_theme->GetScreen())) {
        current_theme = dark_theme;
    } else {
        current_theme = current_theme_;
    }

    DisplayLockGuard lock(this);
    if (!lock || current_theme == nullptr) {
        return false;
    }

    auto emoji_font = current_theme->GetEmojiFont();
    auto icon_font = current_theme->GetIconFont();

    if (emotion != nullptr && strcmp(emotion, "none") == 0) {
        lv_obj_add_flag(emotion_label_, LV_OBJ_FLAG_HIDDEN);
        return true;
    }

    const char* utf8 = nullptr;
    const LvglFont* font = nullptr;
    if (emotion != nullptr) {
        utf8 = noto_emoji_get_utf8(emotion);
        if (utf8 != nullptr) {
            font = emoji_font.get();
        }
    }

    if (utf8 == nullptr) {
        utf8 = MATERIAL_SYMBOLS_ROBOT_2;
        font = current_theme->GetLargeIconFont().get();
    }

    lv_label_set_text(emotion_label_, utf8);
    lv_obj_set_style_text_font(emotion_label_, font, 0);
    lv_obj_remove_flag(emotion_label_, LV_OBJ_FLAG_HIDDEN);

    return true;
}

void OledDisplay::SetTheme(LvglTheme* theme) {
    auto& theme_manager = LvglThemeManager::GetInstance();
    current_theme_ = theme_manager.GetTheme(theme->GetName());

    DisplayLockGuard lock(this);
    if (!lock || current_theme_ == nullptr) {
        return;
    }

    auto text_font = current_theme_->GetTextFont();
    auto icon_font = current_theme_->GetIconFont();
    auto large_icon_font = current_theme_->GetLargeIconFont();

    if (container_) {
        lv_obj_set_style_text_font(container_, text_font.get(), 0);
    }
    if (emotion_label_) {
        lv_obj_set_style_text_font(emotion_label_, large_icon_font.get(), LV_STATE_DEFAULT);
    }
    if (chat_message_label_) {
        lv_obj_set_style_text_font(chat_message_label_, text_font.get(), 0);
    }
    if (status_label_) {
        lv_obj_set_style_text_font(status_label_, text_font.get(), 0);
    }
    if (network_label_) {
        lv_obj_set_style_text_font(network_label_, icon_font.get(), 0);
    }
    if (notification_label_) {
        lv_obj_set_style_text_font(notification_label_, text_font.get(), 0);
    }
    if (mute_label_) {
        lv_obj_set_style_text_font(mute_label_, icon_font.get(), 0);
    }
    if (battery_label_) {
        lv_obj_set_style_text_font(battery_label_, icon_font.get(), 0);
    }

    Display::SetTheme(theme);
}
