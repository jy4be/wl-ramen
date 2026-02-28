#include "icon.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void ic_freeImage(struct Image toFree){
    stbi_image_free(toFree.pixels);
}

struct Image ic_loadImageFromDisk(const char *path){
    struct Image image = {0};
    int w = 0, h = 0, cpp = 0;
    image.pixels = stbi_load(path, &w, &h, &cpp, 4);
    image.size.x = w;
    image.size.y = h;
    return image;
}

void ic_rescaleImage(struct Image* image, struct vector dimensions){
    uint8_t *newPixels = malloc(dimensions.x * dimensions.y * sizeof(uint8_t) * 4);
    for (int32_t x = 0; x < dimensions.x; x++){
        for (int32_t y = 0; y < dimensions.y; y++){
            uint32_t transformedX = (x / (float) dimensions.x) * image->size.x;
            uint32_t transformedY = (y / (float) dimensions.y) * image->size.y;
            uint32_t originalIndex = 
                (transformedY * image->size.x + transformedX) * 4 ;
            for (int clrPart = 0; clrPart < 4; clrPart++){
                newPixels[(y * dimensions.x + x) * 4 + clrPart] = 
                    image->pixels[ originalIndex + clrPart];
            }
        }
    }
    ic_freeImage(*image);
    image->pixels = newPixels;
    image->size = dimensions;
}

void ic_fillImageBufferFromPaths(struct Image *outBuffer, char** imagePaths, size_t amountOfPaths){
    for (size_t i = 0; i < amountOfPaths; i++){
        if(!imagePaths[i])
            continue;

        outBuffer[i] = ic_loadImageFromDisk(imagePaths[i]);
        if (!outBuffer[i].pixels){
            fprintf(stderr, "Could not load Image at %s\n", imagePaths[i]);
        }
    }
}

void ic_resizeImagesInBuffer(struct Image *buffer, size_t amountOfImages, struct vector newSize){
    for (size_t i = 0; i < amountOfImages; i++){
        if (buffer[i].pixels)
            ic_rescaleImage(&buffer[i], newSize);
    }
}

void ic_freeImages(struct Image images[], uint16_t imagesAmount) {
    for (int i = 0; i < imagesAmount; i++){
        if (images[i].size.x && images[i].size.y)
            ic_freeImage(images[i]);
    }
}
