#ifndef DIALPAINTING_H_
#define DIALPAINTING_H_
#include "types.h"

void dp_paintBasedial(
    struct dial* dial, 
    struct screenData screen, 
    uint32_t selectedTab);
void dp_paintDivisions(
    struct dial* Dial, 
    struct screenData screen);

#endif
