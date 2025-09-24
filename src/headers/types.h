#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stddef.h>

struct screenData {
    uint16_t width;
    uint16_t height;
    uint32_t *pixels;
};

struct vector {
    int16_t x;
    int16_t y;
};

struct bannerInfo{
    struct vector position;
    struct vector size;
    size_t stringIndex;
    uint32_t bgColour;
    uint32_t fgColour;
};
struct bannerSettings {
    struct vector position;
    struct vector size;
    uint32_t bgColour;
    uint32_t fgColour;
};

struct dial {
    uint16_t divisionsAmount;
    struct vector center;
    uint16_t outerRadius;
    uint16_t innerRadius;
    struct vector outerDivisions[16];
    struct vector innerDivisions[16];
    struct vector normals[16];
    const char** strings;
    struct bannerSettings banner;
};

struct dialSettings {
    struct vector screenDimensions;
    uint16_t divisionsAmount;
    uint16_t outerRadius;
    uint16_t innerRadius;
    const char** strings;
    struct bannerSettings banner;
};



#endif
