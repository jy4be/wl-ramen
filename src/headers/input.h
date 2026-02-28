#ifndef INPUT_H
#define INPUT_H

#include "types.h"

enum OPTION_PARSING{
    OPT_OK,
    OPT_ERR,
    OPT_USAGE
};


void in_freeInputStrings(struct inputStrings strings);
struct inputStrings in_readstdin(void);
enum OPTION_PARSING in_readOptionsAndSetSettings(
        struct dialSettings* settings,
        int argc,
        const char **argv);

#endif
