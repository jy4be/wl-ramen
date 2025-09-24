#include <stddef.h>
#include <stdint.h>
#include "basewl.h"
#include <stdio.h>
#include "math_util.h"
#include "types.h"
#include "graphics.h"
#include "text.h"

struct dial Dial;
struct stringPixelBuffers StringBuffers;




void setupDial(struct dial *dial, struct dialSettings settings);
void renderBanner(struct screenData screen, struct stringPixelBuffers pixelBuffers, struct bannerSettings banner, size_t stringIndex);

struct bwl_command update(struct screenData screen, struct bwl_pointer_info pointer) 
{
    size_t currentDial = mu_divisionIndexFromSample(pointer.position, Dial);
    for (int x = screen.width/2 - Dial.outerRadius; x < screen.width/2 + Dial.outerRadius; x++) {
        for (int y = screen.height/2 - Dial.outerRadius; y < screen.height/2 + Dial.outerRadius; y++) {
            struct vector current = {x, y};
            if (mu_isInCircle(Dial.outerRadius, current, Dial.center) && 
                !mu_isInCircle(Dial.innerRadius, current, Dial.center))
            {
                if (mu_arePointsInSameDivision(pointer.position, current, Dial))
                    g_setPixel(screen, x, y, 0xFF000000);
                else
                    g_setPixel(screen, x, y, 0xFFFFFFFF);
            }
        }
    }

    for (int i = 0; i < Dial.divisionsAmount; i++){
       g_line(screen, Dial.outerDivisions[i], Dial.innerDivisions[i], 0xFF000000);
    }
    renderBanner(
            screen, 
            StringBuffers, 
            Dial.banner, 
            mu_divisionIndexFromSample(pointer.position, Dial));

    if (pointer.isLeftPressed){
        printf("%s\n", Dial.strings[currentDial]);
        return (struct bwl_command) {
            .shouldClose = true};
    }
    return (struct bwl_command) {
        .shouldClose = pointer.isRightPressed};
}

int main(int argc, const char **argv){
    /*const char *strings[3] = {
        "BarFoo",
        "FarBar",
        "Foobar"};*/
    
    StringBuffers = pixelBufferFromStrings(argv + 1, argc - 1, (struct vector) {200, 100});
    struct vector screenDimensions = {.x = 640, .y = 600};
    setupDial(&Dial, 
        (struct dialSettings) {
            .screenDimensions = screenDimensions,
            .outerRadius = 200,
            .innerRadius = 100,
            .divisionsAmount = argc - 1,
            .strings = argv + 1,
            .banner = (struct bannerSettings) {
                .size = (struct vector) {200, 50},
                .position = (struct vector) {
                    screenDimensions.x / 2 - 100,
                    screenDimensions.y / 2 + 250},
                .bgColour = 0xFFFFFFFF,
                .fgColour = 0xFF808080}});

    bwl_init(
        (struct bwl_settings){
            .update = update,
            .width = screenDimensions.x,
            .height = screenDimensions.y});

    freePixelBuffer(StringBuffers);
    return 0;
}


void setupDial(struct dial *dial, struct dialSettings settings){
    dial->divisionsAmount = settings.divisionsAmount;
    dial->innerRadius = settings.innerRadius;
    dial->outerRadius = settings.outerRadius;
    dial->center = (struct vector) {
        .x = settings.screenDimensions.x / 2,
        .y = settings.screenDimensions.y / 2};
    dial->strings = settings.strings;
    dial->banner = settings.banner;
    
    mu_generateRadialDivisions(dial);
    mu_generateNormals(dial);
}

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
