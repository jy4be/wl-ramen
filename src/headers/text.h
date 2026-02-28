#ifndef TEXT_H
#define TEXT_H
#include "types.h"
#include <freetype/freetype.h>


struct stringPixelBuffers {
    size_t buffersAmount;
    uint8_t **buffers;
    struct vector stringPixelDimensions[16];
    uint32_t bearings[16];
};

struct stringPixelBuffers txt_pixelBufferFromStrings(
        char** strings, 
        size_t stringsAmount, 
        uint32_t fontSize,
        const char *fontFile);
void txt_freePixelBuffer(struct stringPixelBuffers);

#endif
