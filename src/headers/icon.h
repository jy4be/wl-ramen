#ifndef ICON_H
#define ICON_H

#include "types.h"


void ic_rescaleImage(struct Image* image, struct vector dimensions);
struct Image ic_loadImageFromDisk(const char *path);
void ic_freeImage(struct Image toFree);
void ic_fillImageBufferFromPaths(
    struct Image *outBuffer, 
    char** imagePaths, 
    size_t amountOfPaths);
void ic_resizeImagesInBuffer(
    struct Image *buffer, 
    size_t amountOfImages, 
    struct vector newSize);
void ic_freeImages(struct Image images[], uint16_t imagesAmount);

#endif
