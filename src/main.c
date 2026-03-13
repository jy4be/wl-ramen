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
 * //-B Banner Background Image
 * -i Icon Size
 * -s solid color
 */


//TODO: Read monitor size and adjust window

/*extern char binary_LiberationSans_Regular_ttf_size[];
extern char binary_LiberationSans_Regular_ttf_start[];
extern char binary_LiberationSans_Regular_ttf_end[];*/

struct dialSettings Settings = 
{
    .dialWidth = 100,
    .innerRadius = 100,
    .imageDimensions = (struct vector)
        {.x = 64, .y = 64},
    .bgColour = 0xFFE0E0E0,
    .selectColour = 0xFF202020,
    .fadeOutDial = true,
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
    enum OPTION_PARSING optionSuccess = 
        in_readOptionsAndSetSettings(&Settings, argc, argv);
    if (optionSuccess == OPT_USAGE || optionSuccess == OPT_ERR)
    {
        usage();
        return 1;
    }

    struct inputStrings strings = in_readstdin();
    if (strings.amount == 0){
        fprintf(stderr, "No selections given on STDIN\n");
        usage();
        return 1;
    }

    Settings.divisionsAmount = strings.amount;
    Settings.strings = strings.lines;
    Settings.imagePaths = strings.imagePaths;
    setupDial(&Dial, Settings);
    
    bwl_init(
        (struct bwl_settings){
            .update = update});

    in_freeInputStrings(strings);
    txt_freePixelBuffer(StringBuffers);
    ic_freeImages(Dial.images, Dial.divisionsAmount);
    return 0;
}

struct bwl_command update(
    struct screenData screen, 
    struct bwl_pointer_info pointer)
{
    Dial.center = 
        (struct vector) {screen.width/2, screen.height/2};
    struct vector centerAdjustedPointer =
        mu_vecSub(pointer.position, Dial.center);
    size_t currentDivision = 
        mu_divisionIndexFromSample(centerAdjustedPointer, &Dial);

    if (Dial.lastFrameHighlightedDivison != currentDivision)
    {
        g_clearScreen(screen);
        dp_paintBasedial(&Dial, screen, currentDivision);
        dp_paintDivisions(&Dial, screen);

        BA_renderBanner(
            screen, 
            StringBuffers, 
            Dial.banner, 
            currentDivision);
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
        "Usage: wl-ramen [-f Font] [-r Radius] [-d Width] [-cb BackgroudnColour] [-cs ForegroundColour]\n"
        "                [-b center|below|none] [-fs fontSize] [-i iconSize] [-s]\n"
        "Generates a radial menu from new-line seperated values read from STDIN. Prints user selection to STDOUT.\n"
        "Selection can be done with a left click, canceling the dial is done via right clicking.\n"
        "The read values can either be simple strings seperated by newlines, or pairs of strings and paths to images,\n"
        "seperated by a colon (see below for Examples).\n\n"

        "Options.\n"
        " -f font:              Specifies font, takes path to font file\n"
        " -r radius:            Inner radius of the dial\n"
        " -d width:             Width of the dial\n"
        " -cb colour:           Background colour\n"
        " -cs colour:           Foreground/selection colour\n"
        " -b center|below|none: Position to display the banner\n"
        " -bs size:             Font size, in pixels\n"
        " -s:                   Paint solid colour instead of fading out\n"
        " -i size:              Icon size, in pixels (Icon will be resized to square dimensions)\n\n"
        "Colour is specified in hexadecimal RGB or ARGB format. Either '0x', '#' or nothing may be prepended.\n\n"

        "Examples.\n"
        "echo \"Options 1\\nOption 2\\nOption 2\" | wl-ramen                show a simple menus with three options\n"
        "echo \"1:image1.png\\n2:image2.png\\n3:image3.png\" | wl-ramen     show a menu with three options, also display images on dial\n\n"
        
        "Requirements.\n"
        "The freetype2 library must be installed, and the your wayland compositor has to support the Layer Shell Protocol.\n\n"
        "Displays images via the brilliant stb images libraray: https://github.com/nothings/stb.\n"
        "This application comes bundeled with the LiberationSans Regular font, for licensing\n"
        "information see https://openfontlicense.org/.\n");
}


void setupDial(struct dial *dial, struct dialSettings settings)
{
    dial->divisionsAmount = settings.divisionsAmount;
    dial->innerRadius = settings.innerRadius;
    dial->outerRadius = settings.innerRadius + settings.dialWidth;
    dial->strings = settings.strings;
    dial->imageDimensions = settings.imageDimensions;
    dial->selectColour = settings.selectColour;
    dial->bgColour = settings.bgColour;
    dial->fadeOutDial = settings.fadeOutDial;
    dial->banner = BA_bannerInfoFromSettings(
        settings.banner, 
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

