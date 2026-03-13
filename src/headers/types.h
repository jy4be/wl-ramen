#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_DIVISIONS 16 

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
    uint8_t *pixels;
};

struct screenData {
    uint16_t width;
    uint16_t height;
    uint32_t *pixels;
};

struct bannerInfo{
    const char* fontFile;
    const char* bgImage;
    enum BANNER_POSITON position;
    int32_t yOffset;
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
    bool fadeOutDial;
    char** strings;
    uint32_t selectColour;
    uint32_t bgColour;
    uint32_t lastFrameHighlightedDivison;
    struct bannerInfo banner;
    struct vector imageDimensions;
    struct vector outerDivisions[MAX_DIVISIONS];
    struct vector innerDivisions[MAX_DIVISIONS];
    struct vector tabsCenters[MAX_DIVISIONS];
    struct vector normals[MAX_DIVISIONS];
    struct Image  images[MAX_DIVISIONS];
};

struct dialSettings {
    uint16_t divisionsAmount;
    uint16_t dialWidth;
    uint16_t innerRadius;
    char** strings;
    char** imagePaths;
    bool fadeOutDial;
    struct vector imageDimensions;
    struct bannerSettings banner;
    uint32_t selectColour;
    uint32_t bgColour;
};

struct inputStrings {
    size_t amount;
    char* lines[MAX_DIVISIONS];
    char* imagePaths[MAX_DIVISIONS];
};


#endif
