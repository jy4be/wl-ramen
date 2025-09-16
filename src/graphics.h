#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "types.h"

void g_setPixel(struct screenData screen, int16_t x, int16_t y, uint32_t colour);
void g_line(struct screenData screen, struct vector start, struct vector stop, uint32_t colour);

#endif
