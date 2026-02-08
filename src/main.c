#include <stddef.h>
#include <stdint.h>
#include "basewl.h"
#include <stdio.h>
#include <stdlib.h>
#include "math_util.h"
#include "types.h"
#include "graphics.h"
#include "text.h"
#include "banner.h"

struct dial Dial;
struct stringPixelBuffers StringBuffers;


void setupDial(struct dial *dial, struct dialSettings settings);

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

struct inputStrings readstdin(void)
{
    struct inputStrings strings = {0};
    char *line = NULL;
    size_t linesiz = 0;
    ssize_t len;
 
    for (size_t i = 0; ((len = getline(&line, &linesiz, stdin)) != -1) && strings.amount <= 16; i++) {
        if (line[len - 1] == '\n')
            line[len - 1] = '\0';
        strings.lines[i] = strdup(line);
        strings.amount++;
    }
    free(line);
    
    return strings;
}

void freeInputStrings(struct inputStrings strings) {
    for (size_t i = 0; i < strings.amount;i++){
        free(strings.lines[i]);
    }
}

int main(int argc, const char **argv){
    struct inputStrings strings = readstdin();
    struct vector screenDimensions = {.x = 640, .y = 600};
    setupDial(&Dial, 
        (struct dialSettings) {
            .screenDimensions = screenDimensions,
            .outerRadius = 200,
            .innerRadius = 100,
            .divisionsAmount = strings.amount,
            .strings = strings.lines,
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

    freeInputStrings(strings);
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
    StringBuffers = pixelBufferFromStrings(settings.strings, settings.divisionsAmount, settings.banner.size);
    
    mu_generateRadialDivisions(dial);
    mu_generateNormals(dial);
}

