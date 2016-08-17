#include "finderpattern.h"
#include <cmath>
FinderPattern::FinderPattern()
{
    x=0;
    y=0;
    count=0;
    estimatedModuleSize=0;
}

FinderPattern::FinderPattern(float posX, float posY, float estimatedModuleSize, int count)
{
    x=posX;
    y=posY;
    this->estimatedModuleSize = estimatedModuleSize;
    this->count = count;
}

float FinderPattern::getX()
{
    return x;
}

float FinderPattern::getY()
{
    return y;
}

bool FinderPattern::aboutEquals(float moduleSize, float i, float j)
{
    if (std::abs(i - getY()) <= moduleSize && std::abs(j - getX()) <= moduleSize) {
      float moduleSizeDiff = std::abs(moduleSize - estimatedModuleSize);
      return moduleSizeDiff <= 1.0f || moduleSizeDiff <= estimatedModuleSize;
    }
    return false;
}

void FinderPattern::combineEstimate(float i, float j, float newModuleSize)
{
    int combinedCount = count + 1;
    float combinedX = (count * getX() + j) / combinedCount;
    float combinedY = (count * getY() + i) / combinedCount;
    float combinedModuleSize = (count * estimatedModuleSize + newModuleSize) / combinedCount;

    x = combinedX;
    y = combinedY;
    estimatedModuleSize = combinedModuleSize;
    count = combinedCount;
}
