#include <stddef.h>
#include <stdint.h>
#include "basewl.h"
#include <stdio.h>
#include <math.h>

struct screenData {
    uint16_t width;
    uint16_t height;
    uint32_t *pixels;
};

struct vector {
    uint16_t x;
    uint16_t y;
};

struct vector* generateCircleCenters(struct screenData screen, uint8_t circlesAmount, uint16_t radius, struct vector* outBuffer);

void setPixel(struct screenData screen, uint16_t x, uint16_t y, uint32_t colour);

void drawCircle(struct screenData screen, uint16_t radius, uint16_t x, uint16_t y, uint32_t colour);

bool isInCircle(uint16_t radius, uint16_t x, uint16_t y, uint16_t xOrg, uint16_t yOrg);

struct vector* generateCircleCenters(struct screenData screen, uint8_t circlesAmount, uint16_t radius, struct vector* outBuffer){
    double fullCircle = 2 * 3.14;
    uint16_t centerX = screen.width /2 ;
    uint16_t centerY = screen.height/2 ;

    for (int circleIndex = 0; circleIndex < circlesAmount; circleIndex++){
        double currentRadians = (fullCircle / circlesAmount) * circleIndex;
        double xOff = centerX + sin(currentRadians) * radius;
        double yOff = centerY + cos(currentRadians) * radius;
        outBuffer[circleIndex] = (struct vector) {.x = xOff, .y = yOff};
    }

    return outBuffer;
}

void drawCircle(struct screenData screen, uint16_t radius, uint16_t centerX, uint16_t centerY, uint32_t colour){
    uint16_t topLeftX = centerX - radius < 0 ? 0 : centerX - radius;
    uint16_t topLeftY = centerY - radius < 0 ? 0 : centerY - radius;
    uint16_t bottomRightX = centerX + radius > screen.width ? screen.width : centerX + radius;
    uint16_t bottomRightY = centerY + radius > screen.height ? screen.height : centerY + radius;
    
    for (uint16_t x = topLeftX; x < bottomRightX; x++){
        for (uint16_t y = topLeftY; y < bottomRightY; y++){
            if (isInCircle(radius, x, y, centerX, centerY))
                setPixel(screen, x, y, colour);
            else
                setPixel(screen, x, y, 0x20202020);
        }
    }
}

void setPixel(struct screenData screen, uint16_t x, uint16_t y, uint32_t colour){
    screen.pixels[y * screen.width + x] = colour;
}


bool isInCircle(uint16_t radius, uint16_t x, uint16_t y, uint16_t xOrg, uint16_t yOrg){
    return (xOrg-x)*(xOrg-x) + (yOrg - y)*(yOrg - y) < radius*radius;
}

struct bwl_command update(uint32_t *pixels, uint16_t width, uint16_t height, struct bwl_pointer_info pointer) 
{
    for (size_t i = 0; i < width * height; i++){
        pixels[i] = 0;//0x20202020;
    }

    struct screenData screen = {
        .pixels = pixels,
        .width = width,
        .height = height
    };

    const uint8_t circlesAmount = 5;

    struct vector centers[circlesAmount];
    generateCircleCenters(screen, circlesAmount, 100, centers);

    for (int i = 0; i < circlesAmount; i++){
        bool isHoveringCircle = isInCircle(20, pointer.x, pointer.y, centers[i].x, centers[i].y);
        if (isHoveringCircle)
            drawCircle(screen, 20, centers[i].x, centers[i].y, 0xFFFFFFFF);
        else
            drawCircle(screen, 20, centers[i].x, centers[i].y, 0xFF000000);

        if (isHoveringCircle && pointer.isLeftPressed)
            printf("Circle pressed: %d\n", i);
    }
    //drawCircles(8, 100, (struct screenData){.width = width, .height = height, .pixels = pixels});
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
