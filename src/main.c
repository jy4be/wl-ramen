#include <stddef.h>
#include <stdint.h>
#include "basewl.h"
#include <stdio.h>

bool isInCircle(uint16_t radius, uint16_t x, uint16_t y, uint16_t xOrg, uint16_t yOrg);

bool isInCircle(uint16_t radius, uint16_t x, uint16_t y, uint16_t xOrg, uint16_t yOrg){
    return (xOrg-x)*(xOrg-x) + (yOrg - y)*(yOrg - y) < radius*radius;
}

struct bwl_command update(uint32_t *pixels, uint16_t width, uint16_t height, struct bwl_pointer_info pointer) 
{
    for (size_t i = 0; i < width * height; i++){
        pixels[i] = isInCircle(30, i % width , i / width, 100, 100 ) ? 0xFF000000 : 0x20202020;
    }
    return (struct bwl_command) {
        .shouldClose = pointer.isRightPressed};
}

int main(){
    bwl_init(
        (struct bwl_settings){
            .update = update,
            .width = 640,
            .height = 480});

    return 0;
}
