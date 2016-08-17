#ifndef FINDERPATTERN_H
#define FINDERPATTERN_H


class FinderPattern
{
public:
    FinderPattern();
    FinderPattern(float posX, float posY, float estimatedModuleSize, int count);
    float getX();
    float getY();
    bool aboutEquals(float moduleSize,float i,float j);
    void combineEstimate(float i,float j,float newModuleSize);

    float estimatedModuleSize;
    int count;
    int x;
    int y;
};

#endif // FINDERPATTERN_H
