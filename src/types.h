#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

struct screenData {
    uint16_t width;
    uint16_t height;
    uint32_t *pixels;
};

struct vector {
    int16_t x;
    int16_t y;
};



#endif
