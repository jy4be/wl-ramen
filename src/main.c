#include <stddef.h>
#include <stdint.h>
#include "basewl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graphics.h"
#include "math_util.h"
#include "types.h"
#include "text.h"
#include "banner.h"
#include "icon.h"
#include "input.h"
#include "dialPainting.h"

/* Possbile Options
 * -f Font
 * -r Inner radius
 * -d dial width
 * -cb Backgound colour
 * -cs Selection/text color
 * -b CENTER/BELOW/NONE
 * -bs Font size
 * //-bl Banner Character Space
 * -B Banner Background Image
 * -i Icon Size
 */

extern char binary_LiberationSans_Regular_ttf_size[];
extern char binary_LiberationSans_Regular_ttf_start[];
extern char binary_LiberationSans_Regular_ttf_end[];

struct dialSettings Settings = 
{
    .screenDimensions = (struct vector) {1920, 1080},
    .dialWidth = 100,
    .innerRadius = 100,
    .imageDimensions = (struct vector)
        {.x = 64, .y = 64},
    .bgColour = 0xFFE0E0E0,
    .selectColour = 0xFF202020,
    .banner = (struct bannerSettings) {
        .fontSize = 30,
        .width = 200,
        .position = BANNER_BELOW,
        .fontFile= ""
}};


struct stringPixelBuffers StringBuffers;
struct dial Dial;

void setupDial(struct dial *dial, struct dialSettings settings);
struct bwl_command update(
    struct screenData screen, 
    struct bwl_pointer_info pointer);
void usage(void);

int main(int argc, const char **argv)
{
    struct inputStrings strings = in_readstdin();
    if (strings.amount == 0){
        fprintf(stderr, "No selections given on STDIN\n");
        usage();
        return 1;
    }

    enum OPTION_PARSING optionSuccess = 
        in_readOptionsAndSetSettings(&Settings, argc, argv);
    if (optionSuccess == OPT_USAGE || optionSuccess == OPT_ERR)
    {
        usage();
        return 1;
    }

    Settings.divisionsAmount = strings.amount;
    Settings.strings = strings.lines;
    Settings.imagePaths = strings.imagePaths;
    setupDial(&Dial, Settings);
    
    bwl_init(
        (struct bwl_settings){
            .update = update,
            .width = Settings.screenDimensions.x,
            .height = Settings.screenDimensions.y});

    in_freeInputStrings(strings);
    txt_freePixelBuffer(StringBuffers);
    ic_freeImages(Dial.images, Dial.divisionsAmount);
    return 0;
}

struct bwl_command update(struct screenData screen, struct bwl_pointer_info pointer)
{
    size_t currentDivision = mu_divisionIndexFromSample(pointer.position, &Dial);
    if (Dial.lastFrameHighlightedDivison != currentDivision)
    {
        g_clearScreen(screen);
        dp_paintBasedial(&Dial, screen, currentDivision);
        dp_paintDivisions(&Dial, screen);

        BA_renderBanner(
            screen, 
            StringBuffers, 
            Dial.banner, 
            mu_divisionIndexFromSample(pointer.position, &Dial));
    }

    Dial.lastFrameHighlightedDivison = currentDivision;
    if (pointer.isLeftPressed){
        printf("%s\n", Dial.strings[currentDivision]);
        return (struct bwl_command) {
            .shouldClose = true};
    }
    return (struct bwl_command) {
        .shouldClose = pointer.isRightPressed};
}

void usage(void) 
{
    printf(
        "This is should print the usage"
        "yaay");
}


void setupDial(struct dial *dial, struct dialSettings settings)
{
    dial->divisionsAmount = settings.divisionsAmount;
    dial->innerRadius = settings.innerRadius;
    dial->outerRadius = settings.innerRadius + settings.dialWidth;
    dial->center = (struct vector) {
        .x = settings.screenDimensions.x / 2,
        .y = settings.screenDimensions.y / 2};
    dial->strings = settings.strings;
    dial->imageDimensions = settings.imageDimensions;
    dial->selectColour = settings.selectColour;
    dial->bgColour = settings.bgColour;
    dial->banner = BA_bannerInfoFromSettings(
        settings.banner, 
        settings.screenDimensions, 
        settings.bgColour, 
        settings.selectColour,
        settings.innerRadius + settings.dialWidth);
    ic_fillImageBufferFromPaths(
        dial->images, 
        settings.imagePaths, 
        settings.divisionsAmount);
    ic_resizeImagesInBuffer(
        dial->images, 
        settings.divisionsAmount, 
        settings.imageDimensions);
    
    mu_generateRadialDivisions(dial);
    mu_generateNormals(dial);
    mu_generateTabsCenters(dial);

    StringBuffers = txt_pixelBufferFromStrings(
        settings.strings, 
        settings.divisionsAmount, 
        settings.banner.fontSize, 
        settings.banner.fontFile);
}

