#include "freetype/fttypes.h"
#include <freetype2/ft2build.h>
#include <freetype/freetype.h>
#include <stdio.h>
#include <stdlib.h>
#include "text.h"

struct fontData {
    FT_Library ft;
    FT_Face face;
};

size_t renderFont(struct fontData data, uint8_t* buffer, struct vector bufferDimensions, const char* text){
    char currentChar;
    uint32_t x = 0;
    while ((currentChar = *(text++)) != '\0'){
        FT_UInt glyphIndex = 
            FT_Get_Char_Index(data.face, currentChar);
        if (glyphIndex == 0)
            continue;
        FT_Load_Glyph(data.face, glyphIndex, FT_LOAD_DEFAULT);

        FT_GlyphSlot glyph = data.face->glyph;

        int bbox_ymax = data.face->bbox.yMax / 64;
        int glyph_width = glyph->metrics.width / 64;
        int advance = glyph->metrics.horiAdvance / 64;
        int xOff = (advance - glyph_width) / 2;
        int yOff = bbox_ymax - glyph->metrics.horiBearingY / 64;
        

        FT_Render_Glyph(data.face->glyph, FT_RENDER_MODE_NORMAL);
        //FT_GlyphSlot glyph = data.face->glyph;

        int yMax = (int)glyph->bitmap.rows;
        if (yMax >= bufferDimensions.y)
            yMax = bufferDimensions.y - 1;
        //printf("%d\n", yMax);

        int xMax = (int)glyph->bitmap.width;
        
        for (int i = 0; i < yMax; i++){
            uint32_t rowOffset = i + yOff;
            for (int j = 0; j < xMax; j++){
                uint8_t pixel =
                    glyph->bitmap.buffer[i * glyph->bitmap.pitch + j];

                uint16_t bufferX = x + j + xOff;
                if (bufferX < bufferDimensions.x)
                    buffer[rowOffset * bufferDimensions.x + bufferX] = pixel;
            }
        }
        x += advance;
    }
    return x;
    
}

struct stringPixelBuffers pixelBufferFromStrings(
        char** strings, 
        size_t stringsAmount, 
        struct vector bufferDimensions)
{
    struct stringPixelBuffers pixelBuffers = {
        .buffersAmount = stringsAmount,
        .bufferDimensions = bufferDimensions};
    struct fontData font;
    const char *fontFile = 
        "/usr/share/fonts/liberation/LiberationSans-Regular.ttf";

    int errc;
    if((errc = FT_Init_FreeType(&font.ft)))
    {
        printf("Cannot init Library: 0x%X\n", errc);
        return pixelBuffers;
    }
    if((errc = FT_New_Face(font.ft, fontFile, 0, &font.face)))
    {
        printf("Cannot init face: 0x%X\n", errc);
        return pixelBuffers;
    }
    if((errc = FT_Set_Pixel_Sizes(font.face, 0, 30)))
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
        pixelBuffers.buffers[bufferIndex] = malloc(
                bufferDimensions.x * bufferDimensions.y);
        size_t length = renderFont(
                font, 
                pixelBuffers.buffers[bufferIndex], 
                bufferDimensions, 
                strings[bufferIndex]);
        pixelBuffers.stringPixelLength[bufferIndex] = length;
    }

    FT_Done_FreeType(font.ft);

    return pixelBuffers;
}


void freePixelBuffer(struct stringPixelBuffers buffers){
    for (size_t bufferIndex = 0; 
         bufferIndex < buffers.buffersAmount; 
         bufferIndex++)
    {
        free(buffers.buffers[bufferIndex]);
    }
    free(buffers.buffers);
}
