#include "banner.h"
#include "graphics.h"
#include "math_util.h"
#include "types.h"
#include <stdio.h>

#define BORDER_THICKNESS 2
#define PADDING 5
#define BORDER_OFFSET PADDING * 2 + BORDER_THICKNESS

void BA_renderBanner(
        struct screenData screen,
        struct stringPixelBuffers pixelBuffers,
        struct bannerInfo banner,
        size_t stringIndex)
{
    int32_t yPosition = banner.yOffset;
    switch (banner.position) {
    case BANNER_NONE:
        return;
        break;
    case BANNER_CENTER:
        yPosition = screen.height / 2;
        break;
    case BANNER_BELOW:
        yPosition += screen.height / 2;
        break;
    }

    uint32_t bearing = pixelBuffers.bearings[stringIndex];
    int32_t xMax = 
        pixelBuffers.stringPixelDimensions[stringIndex].x;
    int32_t yMax = 
        pixelBuffers.stringPixelDimensions[stringIndex].y;
    int32_t startingXOffset = screen.width / 2 - xMax / 2 
        - BORDER_THICKNESS - PADDING;
    for ( int i = 0; i < xMax + PADDING * 2; i++) {
        for ( int j = 0; j < yMax + PADDING * 2; j++) {
            g_setPixel(
                screen,
                BORDER_THICKNESS + startingXOffset + i,
                BORDER_THICKNESS + yPosition + j - bearing,
                banner.bgColour);
        }
    }
    struct vector topLeft = {
        .x = startingXOffset,
        .y = yPosition - bearing};
    struct vector topRight = {
        .x = startingXOffset + xMax -1 + BORDER_OFFSET,
        .y = yPosition - bearing};
    struct vector bottomLeft = {
        .x = startingXOffset,
        .y = yPosition + yMax -1 - bearing + BORDER_OFFSET};
    struct vector bottomRight = {
        .x = startingXOffset + xMax -1 + BORDER_OFFSET,
        .y = yPosition + yMax -1 - bearing + BORDER_OFFSET};

    for (int i = 0; i < BORDER_THICKNESS; i++)
    {
        g_line(screen, 
            mu_vecAdd(
                topLeft, 
                (struct vector) {i, BORDER_THICKNESS}),
            mu_vecAdd(
                bottomLeft,
                (struct vector) {i, BORDER_THICKNESS}),
            banner.fgColour);
        g_line(screen, 
             mu_vecAdd(
                 topRight,
                 (struct vector) {i+1, 0}),
             mu_vecAdd(
                 bottomRight,
                 (struct vector) {i+1, 0}),
             banner.fgColour);

        g_line(screen, 
             mu_vecAdd(
                 topLeft,
                 (struct vector) {0, i}),
             mu_vecAdd(
                 topRight,
                 (struct vector) {0, i}),
             banner.fgColour);
        g_line(screen, 
             mu_vecAdd(
                 bottomLeft,
                 (struct vector) {BORDER_THICKNESS, i + 1}),
             mu_vecAdd(
                 bottomRight,
                 (struct vector) {BORDER_THICKNESS, i + 1}),
             banner.fgColour);

    }
    for (int i = 0; i < yMax; i++){
        for (int j = 0; j < xMax; j++){
            if (startingXOffset + j < 0)
                continue;
            uint32_t pixelPosition = 
                i * pixelBuffers.stringPixelDimensions[stringIndex].x + j;
            uint32_t colour = 
                banner.fgColour & (0x00FFFFFF | 
                    pixelBuffers.buffers[stringIndex][pixelPosition] << 24);
            g_setPixel(
                screen,
                PADDING + BORDER_THICKNESS + j + startingXOffset,
                PADDING + BORDER_THICKNESS + i + yPosition - bearing, 
                colour);

        }
    }
}

struct bannerInfo BA_bannerInfoFromSettings(
        struct bannerSettings settings,
        uint32_t bgColour,
        uint32_t fgColour,
        uint32_t outerRadius)
{
    struct bannerInfo info = {0};
    if (settings.position == BANNER_BELOW)
    {
        info.yOffset = outerRadius + settings.fontSize; 
    }
    info.fontFile = settings.fontFile;
    info.bgImage = settings.bgImage;
    info.bgColour = bgColour;
    info.fgColour = fgColour;
    info.position = settings.position;
    return info;
}
