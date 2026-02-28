#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stddef.h>

enum BANNER_POSITON{
    BANNER_NONE,
    BANNER_CENTER,
    BANNER_BELOW
};

struct vector {
    int32_t x;
    int32_t y;
};

struct Image {
    struct vector size;
    uint8_t  *pixels;
};

struct screenData {
    uint16_t width;
    uint16_t height;
    uint32_t *pixels;
};

struct bannerInfo{
    const char* fontFile;
    const char* bgImage;
    uint32_t yPosition;
    uint32_t bgColour;
    uint32_t fgColour;
};
struct bannerSettings {
    const char* fontFile;
    const char* bgImage;
    enum BANNER_POSITON position;
    uint32_t fontSize;
    uint32_t width;
};

struct dial {
    uint16_t divisionsAmount;
    struct vector center;
    uint16_t outerRadius;
    uint16_t innerRadius;
    struct vector imageDimensions;
    struct vector outerDivisions[16];
    struct vector innerDivisions[16];
    struct vector tabsCenters[16];
    struct vector normals[16];
    struct Image  images[16];
    char** strings;
    struct bannerInfo banner;
    uint32_t selectColour;
    uint32_t bgColour;
    uint32_t lastFrameHighlightedDivison;
};

struct dialSettings {
    struct vector screenDimensions;
    uint16_t divisionsAmount;
    uint16_t dialWidth;
    uint16_t innerRadius;
    char** strings;
    char** imagePaths;
    struct vector imageDimensions;
    struct bannerSettings banner;
    uint32_t selectColour;
    uint32_t bgColour;
};

struct inputStrings {
    size_t amount;
    char* lines[16];
    char* imagePaths[16];
};


#endif
