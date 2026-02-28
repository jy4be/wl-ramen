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
        .x = dial->center.x - dial->outerRadius,
        .y = dial->center.y - dial->outerRadius
    };
    struct vector bottomRight =
    {
        .x = dial->center.x + dial->outerRadius,
        .y = dial->center.y + dial->outerRadius
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
    if (mu_isInCircle(dial->outerRadius, currentPixel, dial->center) && 
        !mu_isInCircle(dial->innerRadius, currentPixel, dial->center))
    {
        bool isPointerInDivision = 
            mu_divisionIndexFromSample(
                currentPixel, 
                dial) 
            == selectedTab;
        g_setPixel(
            screen, currentPixel.x, currentPixel.y, 
            isPointerInDivision ? 
                dial->selectColour : 
                dial->bgColour);
    }
}

void dp_paintDivisions(struct dial* Dial, struct screenData screen)
{
    for (int i = 0; i < Dial->divisionsAmount; i++)
    {
        g_line(
            screen, 
            Dial->outerDivisions[i], 
            Dial->innerDivisions[i],
            Dial->selectColour);
       if (Dial->images[i].size.x && Dial->images[i].size.y)
       {
           g_drawImage(
               screen, Dial->images[i], 
               mu_vecSub(Dial->tabsCenters[i], 
                   mu_vecScale(Dial->images[i].size, 1.0/2)));
       }
    }
}
