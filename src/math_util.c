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

struct vector mu_vecScale(struct vector v, double scalar){
    return (struct vector) {
        .x = v.x * scalar,
        .y = v.y * scalar};
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

    for (int circleIndex = 0; 
        circleIndex < dial->divisionsAmount; 
        circleIndex++)
    {
        double currentRadians = 
            (fullCircle / dial->divisionsAmount) * circleIndex;
        double xOff = sin(currentRadians);
        double yOff = cos(currentRadians);

        dial->outerDivisions[circleIndex] = (struct vector) {
            .x = xOff * dial->outerRadius,
            .y = yOff * dial->outerRadius};
        dial->innerDivisions[circleIndex] = (struct vector) {
            .x = xOff * dial->innerRadius,
            .y = yOff * dial->innerRadius};
    }
}
struct vector* mu_generateNormals(struct dial *divisions)
{
    for (int i = 0; i < divisions->divisionsAmount; i++){
        divisions->normals[i] =
            mu_rot90Deg(
                mu_vecScale(divisions->outerDivisions[i], -1));
    }
    return divisions->normals;
}

void mu_generateTabsCenters(struct dial *dial){
    double fullCircle = 2 * 3.14;
    uint32_t centerRadius = 
        (dial->outerRadius + dial->innerRadius) / 2;

    for (int circleIndex = 0; 
        circleIndex < dial->divisionsAmount; 
        circleIndex++)
    {
        double currentRadians = 
            (fullCircle / dial->divisionsAmount) * 
                (circleIndex + 0.5);
        double xOff = sin(currentRadians);
        double yOff = cos(currentRadians);
        dial->tabsCenters[circleIndex] = (struct vector) {
            .x = xOff * centerRadius,
            .y = yOff * centerRadius};
    }
}

bool mu_isInDivision(
    struct vector sample, 
    struct dial *dial, 
    uint16_t divisionIndex)
{
    uint16_t nextDivisionIndex = 
        (divisionIndex + 1) % dial->divisionsAmount;

    struct vector normal1 = dial->normals[divisionIndex];
    struct vector normal2 = dial->normals[nextDivisionIndex];
        mu_vecSub(dial->center, sample);
        
    if (mu_dotProduct(normal1, sample) > 0 && 
        mu_dotProduct(normal2, sample) <=0)
    {
        return true;
    }
    return false;
}

size_t mu_divisionIndexFromSample(
    struct vector sample, 
    struct dial *dial)
{
    // Quick and dirty heuristic guess that the current 
    // division is the same as the one from the last call
    static int lastCallindex = 0;
    for (int i = 0; i < dial->divisionsAmount; i++){
        int likelyIndex = (i + lastCallindex) % dial->divisionsAmount;
        if (mu_isInDivision(sample, dial, likelyIndex))
        {
            lastCallindex = likelyIndex;
            return likelyIndex;
        }
    }
    return 0;
}

bool mu_arePointsInSameDivision(
    struct vector p1, 
    struct vector p2, 
    struct dial *dial)
{
    if (dial->divisionsAmount == 1){
        return true;
    }

    for (int i = 0; i < dial->divisionsAmount; i++){
        uint16_t nextDivisionIndex = 
            (i + 1) % dial->divisionsAmount;

        struct vector normal1 = dial->normals[i];
        struct vector normal2 = dial->normals[nextDivisionIndex];

        if (mu_dotProduct(normal1, p1) > 0 && 
            mu_dotProduct(normal2, p1) <=0 &&
            mu_dotProduct(normal1, p2) > 0 && 
            mu_dotProduct(normal2, p2) <=0)
        {
            return true;
        }
    }
    return false;
}

bool mu_isInCircle(uint16_t radius, struct vector sample){
    int32_t x = sample.x;
    int32_t y = sample.y;
    return x*x + y*y < radius*radius;
}
