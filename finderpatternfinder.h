#ifndef FINDERPATTERNFINDER_H
#define FINDERPATTERNFINDER_H

#include <opencv2/video/background_segm.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "finderpattern.h"
#include <QList>
class FinderPatternFinder
{
public:
    FinderPatternFinder(cv::Mat& srcOri);

    cv::Mat src;
    std::vector<FinderPattern> possibleCenters;
    int crossCheckStateCount[5];

    void find();
    bool foundPatternCross(int* stateCount);
    bool handlePossibleCenter(int* stateCount,int i,int j);
    float centerFromEnd(int* stateCount, int end);
    float crossCheckVertical(int startI,int cneterJ,
                             int maxCount,int originalStateCountTotal );
    float crossCheckHorizontal(int startJ,int centerI, int maxCount,
                               int originalStateCountTotal);
    bool crossCheckDiagonal(int startI, int centerJ, int maxCount, int originalStateCountTotal);

};

#endif // FINDERPATTERNFINDER_H
