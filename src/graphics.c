#include "graphics.h"
#include "types.h"
#include <stdbool.h>
#include <stdio.h>

#define abs(a) (a) < 0 ? -(a) : (a)

void g_setPixel(struct screenData screen, int16_t x, int16_t y, uint32_t colour){
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x >= screen.width) x = screen.width-1;
    if (y >= screen.height) y = screen.height-1;
    screen.pixels[y * screen.width + x] = colour;
}

void g_line(struct screenData screen, struct vector start, struct vector stop, uint32_t colour){
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
