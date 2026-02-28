#include "banner.h"
#include "graphics.h"
#include "types.h"
#include <stdio.h>

void BA_renderBanner(
        struct screenData screen,
        struct stringPixelBuffers pixelBuffers,
        struct bannerInfo banner,
        size_t stringIndex)
{
    uint32_t bearing = pixelBuffers.bearings[stringIndex];
    int32_t xMax = pixelBuffers.stringPixelDimensions[stringIndex].x;
    int32_t yMax = pixelBuffers.stringPixelDimensions[stringIndex].y;
    int32_t startingXOffset = screen.width / 2 - xMax / 2;
    for ( int i = 0; i < xMax; i++) {
        for ( int j = 0; j < yMax; j++) {
            g_setPixel(
                    screen,
                    startingXOffset + i,
                    banner.yPosition + j - bearing,
                    banner.bgColour);
        }
    }
    for (int i = 0; i < yMax; i++){
        for (int j = 0; j < xMax; j++){
            if (startingXOffset + j < 0)
                continue;
            uint32_t pixelPosition = 
                i * pixelBuffers.stringPixelDimensions[stringIndex].x + j;
            uint32_t colour = (banner.fgColour & 0x00FFFFFF) | pixelBuffers.buffers[stringIndex][pixelPosition] << 24;
            g_setPixel(
                screen,
                j + startingXOffset,
                i + banner.yPosition - bearing, 
                    colour);

        }
    }
}

struct bannerInfo BA_bannerInfoFromSettings(
        struct bannerSettings settings,
        struct vector screenDimensions,
        uint32_t bgColour,
        uint32_t fgColour,
        uint32_t outerRadius)
{
    struct bannerInfo info = {0};
    info.fontFile = settings.fontFile;
    info.bgImage = settings.bgImage;
    info.bgColour = bgColour;
    info.fgColour = fgColour;
    switch (settings.position) {
    case BANNER_NONE:
        info.yPosition = -1;
        break;
    case BANNER_CENTER:
        info.yPosition = screenDimensions.y / 2;
        //info.yPosition -= (settings.fontSize)/2; 
        break;
    case BANNER_BELOW:
        info.yPosition = screenDimensions.y / 2;
        info.yPosition += outerRadius + settings.fontSize; 
        break;
    }

    return info;
}
