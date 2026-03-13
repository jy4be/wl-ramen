#include "graphics.h"
#include "types.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define abs(a) ((a) < 0 ? -(a) : (a))
#define clamp8Bit(x) ((x) > 0xff ? 0xff : (x))

uint32_t interpolateBetweenColours(uint32_t a, uint32_t b);

void g_clearScreen(struct screenData screen){
    memset(screen.pixels, 0, screen.height * screen.width * 4);
}

void g_setPixel(
    struct screenData screen, 
    int16_t x, 
    int16_t y, 
    uint32_t colour)
{
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x >= screen.width) x = screen.width-1;
    if (y >= screen.height) y = screen.height-1;
    uint32_t originalColour = screen.pixels[y * screen.width + x];
    screen.pixels[y * screen.width + x] = 
        interpolateBetweenColours(originalColour,colour);
}

uint32_t interpolateBetweenColours(uint32_t a, uint32_t b){
    if ((b>>24) == 0xff || (a>>24) == 0)
    {
        return b;
    }
    uint8_t ar = (a >> 16) & 0xFF;
    uint8_t ag = (a >> 8) & 0xFF;
    uint8_t ab = (a >> 0) & 0xFF;
    uint8_t aa = (a >> 24) & 0xFF;
    uint8_t br = (b >> 16) & 0xFF;
    uint8_t bg = (b >> 8) & 0xFF;
    uint8_t bb = (b >> 0) & 0xFF;
    uint8_t ba = (b >> 24) & 0xFF;


    double opacity = ba / 255.0;
    //double opacity = NORMALIZED_OPACITY[ba];

    uint8_t newr = ar + ((br-ar) * opacity);
    uint8_t newg = ag + ((bg-ag) * opacity);
    uint8_t newb = ab + ((bb-ab) * opacity);
    uint8_t newa = clamp8Bit(aa + (abs(ba-aa) * opacity));

    return (newa << 24) | (newr << 16) | (newg << 8) | newb;
}

void g_line(
    struct screenData screen, 
    struct vector start, 
    struct vector stop, 
    uint32_t colour)
{
    struct vector delta = {
        .x = abs(start.x - stop.x),
        .y = -abs(start.y - stop.y)
    };

    struct vector step = {
        .x = start.x < stop.x ? 1 : -1,
        .y = start.y < stop.y ? 1 : -1,
    };

    int16_t error = delta.x + delta.y;
    struct vector current = start;

    while (true){
        g_setPixel(screen, current.x, current.y, colour);
        int16_t doubleError = 2 * error;
        if (doubleError >= delta.y) {
            if (current.x == stop.x)
                break;
            error += delta.y;
            current.x += step.x;
        }
        if (doubleError <= delta.x) {
            if (current.y == stop.y)
                break;
            error += delta.x;
            current.y += step.y;
        }
    }
}

void g_drawImage(
        struct screenData screen, 
        struct Image toDraw, 
        struct vector position)
{
    for (int32_t uvX = 0; uvX < toDraw.size.x; uvX++){
        for (int32_t uvY = 0; uvY < toDraw.size.y; uvY++){
            uint32_t pixel = 
                toDraw.pixels[(uvY * 4) * toDraw.size.y + (uvX * 4)] << 16|
                toDraw.pixels[(uvY * 4) * toDraw.size.y + (uvX * 4) + 1] << 8 |
                toDraw.pixels[(uvY * 4) * toDraw.size.y + (uvX * 4) + 2] << 0 |
                toDraw.pixels[(uvY * 4) * toDraw.size.y + (uvX * 4) + 3] << 24;
            g_setPixel(screen, position.x + uvX, position.y + uvY, pixel);
        }
    }
}

