#include "dialPainting.h"
#include "math_util.h"
#include "graphics.h"

void drawPixelInDial(
    struct dial* dial, 
    struct screenData screen, 
    uint32_t selectedTab, 
    struct vector currentPixel);


void dp_paintBasedial(
    struct dial* dial, 
    struct screenData screen, 
    uint32_t selectedTab)
{
    if (dial->lastFrameHighlightedDivison == selectedTab)
    {
        return;
    }
    struct vector topLeft =
    {
        .x = -dial->outerRadius,
        .y = -dial->outerRadius
    };
    struct vector bottomRight =
    {
        .x = dial->outerRadius,
        .y = dial->outerRadius
    };

    for (int x = topLeft.x; x < bottomRight.x; x++) 
    {
        for (int y = topLeft.y; y < bottomRight.y; y++) 
        {
            struct vector current = {x, y};
            drawPixelInDial(dial, screen, selectedTab, current);
        }
    }
}

void drawPixelInDial(
    struct dial* dial, 
    struct screenData screen, 
    uint32_t selectedTab, 
    struct vector currentPixel)
{
    int x = currentPixel.x;
    int y = currentPixel.y;
    uint8_t fade;
    if (dial->fadeOutDial)
    {
        fade = ~(0xFF * (x * x + y * y) / 
            (dial->outerRadius * dial->outerRadius));
    }
    else
    {
        fade = 0xFF;
    }
    if (mu_isInCircle(dial->outerRadius, currentPixel) && 
        !mu_isInCircle(dial->innerRadius, currentPixel))
    {
        bool isPointerInDivision = 
            mu_divisionIndexFromSample(
                currentPixel, 
                dial) 
            == selectedTab;
        g_setPixel(
            screen, 
            currentPixel.x + dial->center.x, 
            currentPixel.y + dial->center.y, 
            (0x00FFFFFF | (fade << 24)) & (isPointerInDivision ? 
                dial->selectColour : 
                dial->bgColour));
    }
}

void dp_paintDivisions(struct dial* Dial, struct screenData screen)
{
    for (int i = 0; i < Dial->divisionsAmount; i++)
    {
        g_line(
            screen, 
            mu_vecAdd(Dial->outerDivisions[i], Dial->center), 
            mu_vecAdd(Dial->innerDivisions[i], Dial->center),
            Dial->selectColour);
       if (Dial->images[i].size.x && Dial->images[i].size.y)
       {
           g_drawImage(
               screen, Dial->images[i], 
               mu_vecSub(mu_vecAdd(Dial->tabsCenters[i], Dial->center),
                   mu_vecScale(Dial->images[i].size, 1.0/2)));
       }
    }
}
