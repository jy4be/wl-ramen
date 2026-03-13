#ifndef BANNER_H_
#define BANNER_H_
#include "types.h"
#include "text.h"

struct bannerInfo BA_bannerInfoFromSettings(
    struct bannerSettings settings,
    uint32_t bgColour,
    uint32_t fgColour,
    uint32_t outerRadius);
void BA_renderBanner(
    struct screenData screen, 
    struct stringPixelBuffers pixelBuffers, 
    struct bannerInfo banner, 
    size_t stringIndex);

#endif
