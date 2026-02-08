#ifndef TEXT_H
#define TEXT_H
#include "types.h"
#include <freetype/freetype.h>


struct stringPixelBuffers {
    struct vector bufferDimensions;
    size_t buffersAmount;
    uint8_t **buffers;
    size_t stringPixelLength[16];
};

struct stringPixelBuffers pixelBufferFromStrings( char** strings, size_t stringsAmount, struct vector bufferDimensions);
void freePixelBuffer(struct stringPixelBuffers);

#endif
