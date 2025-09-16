#ifndef MATH_UTIL_H
#define MATH_UTIL_H

#include <stdbool.h>
#include "types.h"

struct radialDivisions {
    uint16_t divisionsAmount;
    struct vector center;
    struct vector* outerDivisions;
    struct vector* innerDivisions;
    struct vector* normals;
};

int32_t mu_dotProduct(struct vector v1, struct vector v2);
struct vector mu_rot90Deg(struct vector v);
struct vector mu_vecAdd(struct vector v1, struct vector v2);
struct vector mu_vecSub(struct vector v1, struct vector v2);

struct vector* mu_generateRadialDivisions(struct vector center, uint8_t divisionsAmount, uint16_t radius, struct vector* outBuffer);
struct vector* mu_generateNormals(struct radialDivisions divisions);
bool mu_isInDivision(struct vector sample, struct radialDivisions divisions, uint16_t divisionIndex);
bool mu_arePointsInSameDivision(struct vector p1, struct vector p2, struct radialDivisions divisions);

bool mu_isInCircle(uint16_t radius, struct vector sample, struct vector circleOrigin);

#endif
