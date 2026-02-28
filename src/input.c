#include "input.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "types.h"


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
 
    for (size_t i = 0; 
         ((len = getline(&line, &linesiz, stdin)) != -1)
            && strings.amount <= 16; 
         i++) 
    {
        if (line[len - 1] == '\n')
            line[len - 1] = '\0';
        strings.lines[i] = strdup(line);
        strings.amount++;
        strings.imagePaths[i] = strchr(strings.lines[i], ':');
        if (strings.imagePaths[i]){
            strings.imagePaths[i][0] = '\0';
            strings.imagePaths[i]++;
        }
    }
    free(line);
    
    return strings;
}
enum BANNER_POSITON bannerPositionFromString(const char* positionString){
    if (!strncmp(positionString, "NONE", 6) || 
        !strncmp(positionString, "none", 6))
        return BANNER_NONE;
    if (!strncmp(positionString, "BELOW", 6) || 
        !strncmp(positionString, "below", 6))
        return BANNER_BELOW;
    return BANNER_CENTER;
}

enum OPTION_PARSING in_readOptionsAndSetSettings(
        struct dialSettings* settings,
        int argc,
        const char **argv)
{
    for (int i = 1; i < argc; i++){
        if (i+1 >= argc)
            return OPT_USAGE;
        if (!strncmp(argv[i], "-f", 2)){
            settings->banner.fontFile = argv[++i];
        }
        else if (!strncmp(argv[i], "-r", 2)){
            settings->innerRadius = strtol(argv[++i], NULL, 0);
        }
        else if (!strncmp(argv[i], "-d", 2)){
            settings->dialWidth = strtol(argv[++i], NULL, 0);
        }
        else if (!strncmp(argv[i], "-cb", 3)){
            settings->bgColour = strtoul(argv[++i], NULL, 0);
        }
        else if (!strncmp(argv[i], "-cs", 3)){
            settings->selectColour = strtoul(argv[++i], NULL, 0);
        }
        else if (!strncmp(argv[i], "-b", 3)){
            settings->banner.position = bannerPositionFromString(argv[++i]);
        }
        else if (!strncmp(argv[i], "-bs", 3)){
            settings->banner.fontSize = strtoul(argv[++i], NULL, 0);
        }
        else if (!strncmp(argv[i], "-B", 2)){
            settings->banner.bgImage = argv[++i];
        }
        else if (!strncmp(argv[i], "-i", 2)){
            uint32_t iconSize = strtol(argv[++i], NULL, 0);
            settings->imageDimensions = (struct vector) {iconSize, iconSize};
        }
        else{
            return OPT_USAGE;
        }

    }

    return OPT_OK;
}
