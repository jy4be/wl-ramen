#ifndef BASEWL_H
#define BASEWL_H

#include <stdbool.h>
#include <stdint.h>
#include "types.h"

struct bwl_pointer_info {
    struct vector position;
    bool isLeftPressed;
    bool isRightPressed;
};

struct bwl_settings {
    uint16_t width;
    uint16_t height;
    struct bwl_command (*update)(struct screenData screen, struct bwl_pointer_info pointer);
};

struct bwl_command {
    bool shouldClose;
};

struct bwl_state;

struct bwl_state *bwl_init(struct bwl_settings settings);
struct bwl_pointer_info bwl_getPointerInfo(struct bwl_state *state);

#endif
