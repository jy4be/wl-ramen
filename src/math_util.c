#include "math_util.h"
#include "types.h"

#include <math.h>

struct vector mu_vecAdd(struct vector v1, struct vector v2){
    return (struct vector) {
        .x = v1.x + v2.x,
        .y = v1.y + v2.y};
}

struct vector mu_vecSub(struct vector v1, struct vector v2){
    return (struct vector) {
        .x = v1.x - v2.x,
        .y = v1.y - v2.y};
}

int32_t mu_dotProduct(struct vector v1, struct vector v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

struct vector mu_rot90Deg(struct vector v){
    return (struct vector) {
        .x = -v.y,
        .y = v.x
    };
}

void mu_generateRadialDivisions(struct dial *dial){
    double fullCircle = 2 * 3.14;

    for (int circleIndex = 0; circleIndex <= dial->divisionsAmount; circleIndex++){
        double currentRadians = (fullCircle / dial->divisionsAmount) * circleIndex;
        double xOff = sin(currentRadians);
        double yOff = cos(currentRadians);

        dial->outerDivisions[circleIndex] = (struct vector) {
            .x = dial->center.x + xOff * dial->outerRadius,
            .y = dial->center.y + yOff * dial->outerRadius};
        dial->innerDivisions[circleIndex] = (struct vector) {
            .x = dial->center.x + xOff * dial->innerRadius,
            .y = dial->center.y + yOff * dial->innerRadius};
    }
}
struct vector* mu_generateNormals(struct dial *divisions)
{
    for (int i = 0; i < divisions->divisionsAmount; i++){
        divisions->normals[i] =
            mu_rot90Deg(mu_vecSub(divisions->outerDivisions[i], divisions->center));
    }
    return divisions->normals;
}

bool mu_isInDivision(struct vector sample, struct dial divisions, uint16_t divisionIndex){
    uint16_t nextDivisionIndex = 
        (divisionIndex + 1) % divisions.divisionsAmount;

    struct vector normal1 = divisions.normals[divisionIndex];
    struct vector normal2 = divisions.normals[nextDivisionIndex];
    struct vector centerToPointer = 
        mu_vecSub(divisions.center, sample);
        
    if (mu_dotProduct(normal1, centerToPointer) > 0 && 
        mu_dotProduct(normal2, centerToPointer) <=0)
    {
        return true;
    }
    return false;
}

size_t mu_divisionIndexFromSample(struct vector sample, struct dial dial){
    for (int i = 0; i < dial.divisionsAmount; i++){
        if (mu_isInDivision(sample, dial, i))
            return i;
    }
    return 0;
}

bool mu_arePointsInSameDivision(struct vector p1, struct vector p2, struct dial divisions)
{
    struct vector center = divisions.center;

    struct vector centerToP1 = mu_vecSub(center, p1);
    struct vector centerToP2 = mu_vecSub(center, p2);

    for (int i = 0; i < divisions.divisionsAmount; i++){
        uint16_t nextDivisionIndex = 
            (i + 1) % divisions.divisionsAmount;

        struct vector normal1 = divisions.normals[i];
        struct vector normal2 = divisions.normals[nextDivisionIndex];

        if (mu_dotProduct(normal1, centerToP1) > 0 && 
            mu_dotProduct(normal2, centerToP1) <=0 &&
            mu_dotProduct(normal1, centerToP2) > 0 && 
            mu_dotProduct(normal2, centerToP2) <=0)
        {
            return true;
        }
    }
    return false;

}

bool mu_isInCircle(uint16_t radius, struct vector sample, struct vector circleOrigin){
    int32_t x = circleOrigin.x-sample.x;
    int32_t y = circleOrigin.y-sample.y;
    return x*x + y*y < radius*radius;
}
