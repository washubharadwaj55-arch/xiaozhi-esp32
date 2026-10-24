#ifndef _DYNAMIC_GLYPH_CACHE_H_
#define _DYNAMIC_GLYPH_CACHE_H_

#include <vector>
#include <cstdint>
#include <cstddef>
#include <lvgl.h>

struct TextGlyph {
    uint32_t codepoint = 0;
    uint16_t box_w = 0;
    uint16_t box_h = 0;
    int16_t ofs_x = 0;
    int16_t ofs_y = 0;
    uint16_t adv_w = 0;
    std::vector<uint8_t> bitmap;
};

template <typename T>
using TextGlyphVector = std::vector<T>;

class DynamicGlyphCache {
public:
    struct Entry {
        uint32_t codepoint = 0;
        uint16_t box_w = 0;
        uint16_t box_h = 0;
        int16_t ofs_x = 0;
        int16_t ofs_y = 0;
        uint16_t adv_w = 0;
        std::vector<uint8_t> bitmap;
        uint32_t last_use = 0;
    };

    DynamicGlyphCache();
    ~DynamicGlyphCache() = default;

    lv_font_t* EnsureFont(const lv_font_t* base_font, uint8_t bpp);
    bool AddGlyphs(const std::vector<TextGlyph>& glyphs);
    void Clear();

private:
    void Rebuild();
    size_t BitmapBytes() const;

    static constexpr size_t kMaxGlyphs = 512;
    static constexpr size_t kMaxBitmapBytes = 64 * 1024;

    bool initialized_ = false;
    bool retain_between_batches_ = false;
    uint8_t bpp_ = 1;
    uint32_t use_counter_ = 0;

    lv_font_t font_{};
    lv_font_fmt_txt_dsc_t dsc_{};

    TextGlyphVector<Entry> entries_;
    TextGlyphVector<uint8_t> bitmap_blob_;
    TextGlyphVector<uint16_t> unicode_list_;
    TextGlyphVector<lv_font_fmt_txt_glyph_dsc_t> glyph_dsc_;
    TextGlyphVector<lv_font_fmt_txt_cmap_t> cmaps_;
};

#endif // _DYNAMIC_GLYPH_CACHE_H_
