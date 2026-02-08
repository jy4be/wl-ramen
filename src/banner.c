#include "banner.h"
#include "graphics.h"

void renderBanner(struct screenData screen, struct stringPixelBuffers pixelBuffers, struct bannerSettings banner, size_t stringIndex){
    for ( int i = 0; i < banner.size.x; i++) {
        for ( int j = 0; j < banner.size.y; j++) {
            g_setPixel(screen, banner.position.x + i, banner.position.y + j, banner.bgColour);
        }
    }
    int32_t xMax = pixelBuffers.stringPixelLength[stringIndex];
    int32_t yMax = banner.size.y;
    int32_t startingXOffset = (banner.size.x / 2) - (pixelBuffers.stringPixelLength[stringIndex] / 2);
    if (xMax > banner.size.x)
        xMax = banner.size.x;
    if (yMax > pixelBuffers.bufferDimensions.y)
        yMax = pixelBuffers.bufferDimensions.y;
    for (int i = 0; i < yMax; i++){
        for (int j = 0; j < xMax; j++){
            if (startingXOffset + j < 0)
                continue;
            g_setPixel(screen, j + banner.position.x + startingXOffset, i + banner.position.y, 
                pixelBuffers.buffers[stringIndex][i * pixelBuffers.bufferDimensions.x + j] ?
                    banner.fgColour :
                    banner.bgColour);

        }
    }
}


