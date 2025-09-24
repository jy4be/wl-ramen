#ifndef MATH_UTIL_H
#define MATH_UTIL_H

#include <stdbool.h>
#include "types.h"

int32_t mu_dotProduct(struct vector v1, struct vector v2);
struct vector mu_rot90Deg(struct vector v);
struct vector mu_vecAdd(struct vector v1, struct vector v2);
struct vector mu_vecSub(struct vector v1, struct vector v2);

void mu_generateRadialDivisions(struct dial *dial);
struct vector* mu_generateNormals(struct dial *divisions);
bool mu_isInDivision(struct vector sample, struct dial divisions, uint16_t divisionIndex);
bool mu_arePointsInSameDivision(struct vector p1, struct vector p2, struct dial divisions);

bool mu_isInCircle(uint16_t radius, struct vector sample, struct vector circleOrigin);
size_t mu_divisionIndexFromSample(struct vector sample, struct dial dial);

#endif
