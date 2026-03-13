#include "input.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "types.h"

uint32_t absInt(int x) {
    return x < 0 ? -x : x;
}


void in_freeInputStrings(struct inputStrings strings) {
    for (size_t i = 0; i < strings.amount;i++){
        free(strings.lines[i]);
    }
}

struct inputStrings in_readstdin(void)
{
    struct inputStrings strings = {0};
    char *line = NULL;
    size_t linesiz = 0;
    ssize_t len;
 
    for (; 
         ((len = getline(&line, &linesiz, stdin)) != -1)
            && strings.amount < MAX_DIVISIONS; 
         strings.amount++) 
    {
        if (line[len - 1] == '\n')
            line[len - 1] = '\0';
        strings.lines[strings.amount] = strdup(line);
        strings.imagePaths[strings.amount] = 
            strchr(strings.lines[strings.amount], ':');
        if (strings.imagePaths[strings.amount])
        {
            strings.imagePaths[strings.amount][0] = '\0';
            strings.imagePaths[strings.amount]++;
        }
    }
    free(line);
    
    return strings;
}
enum BANNER_POSITON bannerPositionFromString(const char* positionString)
{
    if (!strncmp(positionString, "NONE", 6) || 
        !strncmp(positionString, "none", 6))
        return BANNER_NONE;
    if (!strncmp(positionString, "CENTER", 6) || 
        !strncmp(positionString, "center", 6))
        return BANNER_CENTER;
    return BANNER_BELOW;
}

uint32_t readColour(const char* colourString)
{
    uint32_t value;
    uint32_t alphaMask = 0xFF000000;
    if (colourString[0] == '#')
    {
        colourString++;
    }
    else if (colourString[0] == '0' && colourString[1] == 'x')
    {
        colourString += 2;
    }
    value = strtoul(colourString, NULL, 16);

    if (strnlen(colourString, 10) < 7)
    { //In RGB-fotmat, apply alpha channel of 0xFF
        value |= alphaMask;
    }
    return value;
}

enum OPTION_PARSING in_readOptionsAndSetSettings(
    struct dialSettings* settings,
    int argc,
    const char **argv)
{
    for (int i = 1; i < argc; i++){
        if (!strncmp(argv[i], "-s", 2)){
            settings->fadeOutDial = false;
            continue;
        }
        if (i+1 >= argc)
            return OPT_USAGE;
        if (!strncmp(argv[i], "-f", 3)){
            settings->banner.fontFile = argv[++i];
        }
        else if (!strncmp(argv[i], "-r", 3)){
            settings->innerRadius = absInt(strtol(argv[++i], NULL, 0));
        }
        else if (!strncmp(argv[i], "-d", 3)){
            settings->dialWidth = absInt(strtol(argv[++i], NULL, 0));
        }
        else if (!strncmp(argv[i], "-cb", 4)){
            settings->bgColour = readColour(argv[++i]);
        }
        else if (!strncmp(argv[i], "-cs", 4)){
            settings->selectColour = readColour(argv[++i]);
        }
        else if (!strncmp(argv[i], "-b", 3)){
            settings->banner.position = 
                bannerPositionFromString(argv[++i]);
        }
        else if (!strncmp(argv[i], "-bs", 4)){
            settings->banner.fontSize = absInt(strtol(argv[++i], NULL, 0));
        }
        else if (!strncmp(argv[i], "-i", 3)){
            uint32_t iconSize = absInt(strtol(argv[++i], NULL, 0));
            settings->imageDimensions = 
                (struct vector) {iconSize, iconSize};
        }
        else{
            return OPT_USAGE;
        }
    }
    return OPT_OK;
}
