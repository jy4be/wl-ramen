#include <stddef.h>
#include <stdint.h>
#include "basewl.h"
#include <stdio.h>
#include "math_util.h"
#include "types.h"
#include "graphics.h"

struct radialDivisions Divisions = {0};

struct bwl_command update(struct screenData screen, struct bwl_pointer_info pointer) 
{
    uint16_t outerRadius = 100;
    uint16_t innerRadius = 70;

    for (int x = screen.width/2 - outerRadius; x < screen.width/2 + outerRadius; x++) {
        for (int y = screen.height/2 - outerRadius; y < screen.height/2 + outerRadius; y++) {
            struct vector current = {x, y};
            if (mu_isInCircle(outerRadius, current, Divisions.center) && 
                !mu_isInCircle(innerRadius, current, Divisions.center))
            {
                if (mu_arePointsInSameDivision(pointer.position, current,  Divisions))
                    g_setPixel(screen, x, y, 0xFF000000);
                else
                    g_setPixel(screen, x, y, 0xFFFFFFFF);
            }
            else
                g_setPixel(screen, x, y, 0x20202020);
        }
    }

    for (int i = 0; i < Divisions.divisionsAmount; i++){
        g_line(screen, Divisions.outerDivisions[i], Divisions.innerDivisions[i], 0xFF000000);
    }
    return (struct bwl_command) {
        .shouldClose = pointer.isRightPressed};
}

int main(){
    struct vector screenDimensions = {.x = 640, .y = 480};
    struct vector center = {.x = screenDimensions.x/2, .y = screenDimensions.y/2};

    Divisions.divisionsAmount = 5;
    struct vector outer[Divisions.divisionsAmount];
    struct vector inner[Divisions.divisionsAmount];
    struct vector normals[Divisions.divisionsAmount];

    Divisions.center = center;
    Divisions.outerDivisions = outer;
    Divisions.innerDivisions = inner;
    Divisions.normals = normals;
    mu_generateRadialDivisions(center, Divisions.divisionsAmount, 100, outer);
    mu_generateRadialDivisions(center, Divisions.divisionsAmount, 70, inner);
    mu_generateNormals(Divisions);

    bwl_init(
        (struct bwl_settings){
            .update = update,
            .width = screenDimensions.x,
            .height = screenDimensions.y});

    return 0;
}
