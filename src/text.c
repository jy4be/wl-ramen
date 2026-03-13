#include "freetype/fttypes.h"
#include "types.h"
#include <freetype2/ft2build.h>
#include <freetype/freetype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "text.h"

//extern int binary_LiberationSans_Regular_ttf_size;
extern uint8_t _binary_font_LiberationSans_Regular_ttf_start[];
extern uint8_t* _binary_font_LiberationSans_Regular_ttf_end;

struct fontData {
    FT_Library ft;
    FT_Face face;
};

struct layoutInfo {
    uint32_t yBearing;
    uint32_t bufferHeight;
    uint32_t bufferWidth;
};

struct layoutInfo layoutFromString(
    struct fontData data, 
    const char* text)
{
    struct layoutInfo layout = {0};
    char currentChar;
    while ((currentChar = *(text++)) != '\0'){
        FT_UInt glyphIndex = 
            FT_Get_Char_Index(data.face, currentChar);
        if (glyphIndex == 0)
            continue;
        FT_Load_Glyph(data.face, glyphIndex, FT_LOAD_DEFAULT);
        FT_GlyphSlot glyph = data.face->glyph;

        uint32_t height = glyph->metrics.height / 64;
        uint32_t yBearing = glyph->metrics.horiBearingY / 64;

        if (height + (height - yBearing) > layout.bufferHeight){
            layout.bufferHeight = height + (height - yBearing);
        }
        if (yBearing > layout.yBearing)
            layout.yBearing = yBearing;
        layout.bufferWidth += glyph->metrics.horiAdvance / 64;
    }
    return layout;
}

void renderFont(
        struct fontData data,
        uint8_t* buffer,
        struct  layoutInfo layout,
        const char* text)
{
    char currentChar;
    uint32_t x = 0;
    while ((currentChar = *(text++)) != '\0'){
        FT_UInt glyphIndex = 
            FT_Get_Char_Index(data.face, currentChar);
        if (glyphIndex == 0)
            continue;
        FT_Load_Glyph(data.face, glyphIndex, FT_LOAD_DEFAULT);

        FT_GlyphSlot glyph = data.face->glyph;

        int glyph_width = glyph->metrics.width / 64;
        int advance = glyph->metrics.horiAdvance / 64;
        int xOff = (advance - glyph_width) / 2;
        int yOff = layout.yBearing - glyph->metrics.horiBearingY / 64;

        FT_Render_Glyph(data.face->glyph, FT_RENDER_MODE_NORMAL);

        int yMax = (int)glyph->bitmap.rows;
        int xMax = (int)glyph->bitmap.width;
        
        for (int i = 0; i < yMax; i++){
            uint32_t rowOffset = i + yOff;
            for (int j = 0; j < xMax; j++){
                uint8_t pixel =
                    glyph->bitmap.buffer[i * glyph->bitmap.pitch + j];
                uint16_t bufferX = x + j + xOff;

                if (bufferX < layout.bufferWidth && 
                        rowOffset < layout.bufferHeight)
                    buffer[rowOffset * layout.bufferWidth + bufferX] = pixel;
            }
        }
        x += advance;
    }
}

struct stringPixelBuffers txt_pixelBufferFromStrings(
        char** strings, 
        size_t stringsAmount, 
        uint32_t fontSize,
        const char* fontFile)
{
    struct stringPixelBuffers pixelBuffers = {
        .buffersAmount = stringsAmount};
    struct fontData font;

    int errc;
    if((errc = FT_Init_FreeType(&font.ft)))
    {
        printf("Cannot init Library: 0x%X\n", errc);
        return pixelBuffers;
    }
    if (fontFile[0] == '\0'){
        if((errc = FT_New_Memory_Face(
            font.ft, 
            _binary_font_LiberationSans_Regular_ttf_start, 
            _binary_font_LiberationSans_Regular_ttf_end - 
                _binary_font_LiberationSans_Regular_ttf_start, 
            0, 
            &font.face)))
        {
            printf("Cannot init face: 0x%X\n", errc);
            return pixelBuffers;
        }
    }
    else
        if((errc = FT_New_Face(font.ft, fontFile, 0, &font.face)))
        {
            printf("Cannot init face: 0x%X\n", errc);
            return pixelBuffers;
        }
    if((errc = FT_Set_Pixel_Sizes(font.face, 0, fontSize)))
    {
        printf("Cannot set font pixel size: 0x%X\n", errc);
        return pixelBuffers;
    }

    pixelBuffers.buffers = 
        malloc(sizeof(uint8_t*) * stringsAmount);

    for (size_t bufferIndex = 0; 
         bufferIndex < stringsAmount; 
         bufferIndex++)
    {
        struct layoutInfo layout = 
            layoutFromString(font, strings[bufferIndex]);
        pixelBuffers.bearings[bufferIndex] = layout.yBearing;
        pixelBuffers.buffers[bufferIndex] = malloc(
                layout.bufferWidth * layout.bufferHeight);
        memset(
            pixelBuffers.buffers[bufferIndex], 
            0x00, 
            layout.bufferWidth * layout.bufferHeight);
        renderFont(
            font, 
            pixelBuffers.buffers[bufferIndex], 
            layout, 
            strings[bufferIndex]);
        pixelBuffers.stringPixelDimensions[bufferIndex] = 
            (struct vector) {layout.bufferWidth, layout.bufferHeight};
    }

    FT_Done_FreeType(font.ft);

    return pixelBuffers;
}


void txt_freePixelBuffer(struct stringPixelBuffers buffers){
    for (size_t bufferIndex = 0; 
         bufferIndex < buffers.buffersAmount; 
         bufferIndex++)
    {
        free(buffers.buffers[bufferIndex]);
    }
    free(buffers.buffers);
}
