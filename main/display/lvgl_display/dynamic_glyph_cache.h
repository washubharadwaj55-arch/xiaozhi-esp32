#ifndef _DYNAMIC_GLYPH_CACHE_H_
#define _DYNAMIC_GLYPH_CACHE_H_

#include <vector>
#include <cstdint>
#include <lvgl.h>

struct TextGlyph {
    uint32_t unicode;
    uint16_t box_w;
    uint16_t box_h;
    int16_t ofs_x;
    int16_t ofs_y;
    uint16_t adv_w;
    const uint8_t* bitmap;
};

template <typename T>
using TextGlyphVector = std::vector<T>;

class DynamicGlyphCache {
public:
    struct Entry {
        uint32_t unicode;
        lv_font_glyph_dsc_t dsc;
        uint32_t offset;
    };

    DynamicGlyphCache() = default;
    ~DynamicGlyphCache() = default;

    bool AddGlyph(const TextGlyph& glyph);
    bool AddGlyphs(const std::vector<TextGlyph>& glyphs);

    const lv_font_glyph_dsc_t* GetGlyphDsc(uint32_t unicode_letter);
    const uint8_t* GetGlyphBitmap(uint32_t unicode_letter);

private:
    TextGlyphVector<Entry> entries_;
    TextGlyphVector<uint8_t> bitmap_blob_;
    TextGlyphVector<uint16_t> unicode_list_;
    TextGlyphVector<lv_font_fmt_txt_glyph_dsc_t> glyph_dsc_;
    TextGlyphVector<lv_font_fmt_txt_cmap_t> cmaps_;
};

#endif // _DYNAMIC_GLYPH_CACHE_H_
