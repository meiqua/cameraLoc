#include "finderpatternfinder.h"

FinderPatternFinder::FinderPatternFinder(cv::Mat& srcOri)
{
    src = srcOri;
}

void FinderPatternFinder::find()
{
    int maxI = src.rows;
    int maxJ = src.cols;
    int iSkip = 1;

    int stateCount[5];
    for(int i=iSkip-1;i<maxI;i +=iSkip){
        stateCount[0]=stateCount[1]=stateCount[2]
                =stateCount[3]=stateCount[4]=0;
        int currentState=0;
        for(int j=0;j<maxJ;j++){
            if(src.at<uchar>(i,j)==0){
                if((currentState%2)==1){
                   currentState++;
                }
                stateCount[currentState]++;
            }else {
                if((currentState%2)==0){
                    if(currentState==4){
                        if(foundPatternCross(stateCount)){
                            bool confirmed = handlePossibleCenter(stateCount, i, j);
                            if (confirmed) {
                              // Start examining every other line. Checking each line turned out to be too
                              // expensive and didn't improve performance.
                              iSkip = 1;
                            } else {
                              stateCount[0] = stateCount[2];
                              stateCount[1] = stateCount[3];
                              stateCount[2] = stateCount[4];
                              stateCount[3] = 1;
                              stateCount[4] = 0;
                              currentState = 3;
                              continue;
                            }
                            // Clear state to start looking again
                            currentState = 0;
                            stateCount[0] = 0;
                            stateCount[1] = 0;
                            stateCount[2] = 0;
                            stateCount[3] = 0;
                            stateCount[4] = 0;
                          } else { // No, shift counts back by two
                            stateCount[0] = stateCount[2];
                            stateCount[1] = stateCount[3];
                            stateCount[2] = stateCount[4];
                            stateCount[3] = 1;
                            stateCount[4] = 0;
                            currentState = 3;
                          }

                    }else{
                        stateCount[++currentState]++;
                    }
                }else{
                    stateCount[currentState]++;
                }
            }
        }
    }
}

bool FinderPatternFinder::foundPatternCross(int *stateCount)
{
    int totalModuleSize = 0;
    for (int i = 0; i < 5; i++) {
      int count = stateCount[i];
      if (count == 0) {
        return false;
      }
      totalModuleSize += count;
    }
    if (totalModuleSize < 7) {
      return false;
    }
    float moduleSize = totalModuleSize / 7.0f;
    float maxVariance = moduleSize / 2.0f;
    // Allow less than 50% variance from 1-1-3-1-1 proportions
    return
        std::abs(moduleSize - stateCount[0]) < maxVariance &&
        std::abs(moduleSize - stateCount[1]) < maxVariance &&
        std::abs(3.0f * moduleSize - stateCount[2]) < 3 * maxVariance &&
        std::abs(moduleSize - stateCount[3]) < maxVariance &&
            std::abs(moduleSize - stateCount[4]) < maxVariance;
}

bool FinderPatternFinder::handlePossibleCenter(int *stateCount, int i, int j)
{
    int stateCountTotal = stateCount[0] + stateCount[1] + stateCount[2] + stateCount[3] +
            stateCount[4];
        float centerJ = centerFromEnd(stateCount, j);
        float centerI = crossCheckVertical(i, (int) centerJ, stateCount[2], stateCountTotal);
        if (centerI>=0) {
          // Re-cross check
          centerJ = crossCheckHorizontal((int) centerJ, (int) centerI, stateCount[2], stateCountTotal);
          if (centerJ>=0 &&
              (true || crossCheckDiagonal((int) centerI, (int) centerJ, stateCount[2], stateCountTotal))) {
            float estimatedModuleSize = stateCountTotal / 7.0f;
            bool found = false;
            for (int index = 0; index < possibleCenters.size(); index++) {
              FinderPattern center = possibleCenters.at(index);
              // Look for about the same center and module size:
              if (center.aboutEquals(estimatedModuleSize, centerI, centerJ)) {
                possibleCenters[index].combineEstimate(centerI, centerJ, estimatedModuleSize);
                found = true;
                break;
              }
            }
            if (!found) {
                FinderPattern point(centerJ, centerI, estimatedModuleSize,1);
                possibleCenters.push_back(point);
            }
            return true;
          }
        }
        return false;
}

float FinderPatternFinder::centerFromEnd(int *stateCount, int end)
{
    return (end - stateCount[4] - stateCount[3]) - stateCount[2] / 2.0f;
}

float FinderPatternFinder::crossCheckVertical(int startI, int centerJ, int maxCount, int originalStateCountTotal)
{
    int maxI = src.rows;
    crossCheckStateCount[0]=0;
    crossCheckStateCount[1]=0;
    crossCheckStateCount[2]=0;
    crossCheckStateCount[3]=0;
    crossCheckStateCount[4]=0;

    int i = startI;
    while (i >= 0 && src.at<uchar>(i,centerJ)==0) {
      crossCheckStateCount[2]++;
      i--;
    }
    if (i < 0) {
      return -1;
    }
    while (i >= 0 && src.at<uchar>(i,centerJ)>0 && crossCheckStateCount[1] <= maxCount) {
      crossCheckStateCount[1]++;
      i--;
    }
    // If already too many modules in this state or ran off the edge:
    if (i < 0 || crossCheckStateCount[1] > maxCount) {
      return -1;
    }
    while (i >= 0 && src.at<uchar>(i,centerJ)==0 && crossCheckStateCount[0] <= maxCount) {
      crossCheckStateCount[0]++;
      i--;
    }
    if (crossCheckStateCount[0] > maxCount) {
      return -1;
    }

    // Now also count down from center
    i = startI + 1;
    while (i < maxI && src.at<uchar>(i,centerJ)==0) {
      crossCheckStateCount[2]++;
      i++;
    }
    if (i == maxI) {
      return -1;
    }
    while (i < maxI && src.at<uchar>(i,centerJ)>0 && crossCheckStateCount[3] < maxCount) {
      crossCheckStateCount[3]++;
      i++;
    }
    if (i == maxI || crossCheckStateCount[3] >= maxCount) {
      return -1;
    }
    while (i < maxI && src.at<uchar>(i,centerJ)==0 && crossCheckStateCount[4] < maxCount) {
      crossCheckStateCount[4]++;
      i++;
    }
    if (crossCheckStateCount[4] >= maxCount) {
      return -1;
    }

    // If we found a finder-pattern-like section, but its size is more than 40% different than
    // the original, assume it's a false positive
    int stateCountTotal = crossCheckStateCount[0] + crossCheckStateCount[1] + crossCheckStateCount[2] + crossCheckStateCount[3] +
        crossCheckStateCount[4];
    if (5 * std::abs(stateCountTotal - originalStateCountTotal) >= 2 * originalStateCountTotal) {
      return -1;
    }

    return foundPatternCross(crossCheckStateCount) ? centerFromEnd(crossCheckStateCount, i) : -1;
}

float FinderPatternFinder::crossCheckHorizontal(int startJ, int centerI, int maxCount, int originalStateCountTotal)
{
    int maxJ = src.rows;
    crossCheckStateCount[0]=0;
    crossCheckStateCount[1]=0;
    crossCheckStateCount[2]=0;
    crossCheckStateCount[3]=0;
    crossCheckStateCount[4]=0;

    int j = startJ;
    while (j >= 0 && src.at<uchar>(centerI,j)==0) {
      crossCheckStateCount[2]++;
      j--;
    }
    if (j < 0) {
      return -1;
    }
    while (j >= 0 && src.at<uchar>(centerI,j)>0 && crossCheckStateCount[1] <= maxCount) {
      crossCheckStateCount[1]++;
      j--;
    }
    if (j < 0 || crossCheckStateCount[1] > maxCount) {
      return -1;
    }
    while (j >= 0 && src.at<uchar>(centerI,j)==0 && crossCheckStateCount[0] <= maxCount) {
      crossCheckStateCount[0]++;
      j--;
    }
    if (crossCheckStateCount[0] > maxCount) {
      return -1;
    }

    j = startJ + 1;
    while (j < maxJ && src.at<uchar>(centerI,j)==0) {
      crossCheckStateCount[2]++;
      j++;
    }
    if (j == maxJ) {
      return -1;
    }
    while (j < maxJ && src.at<uchar>(centerI,j)>0 && crossCheckStateCount[3] < maxCount) {
      crossCheckStateCount[3]++;
      j++;
    }
    if (j == maxJ || crossCheckStateCount[3] >= maxCount) {
      return -1;
    }
    while (j < maxJ && src.at<uchar>(centerI,j)==0 && crossCheckStateCount[4] < maxCount) {
      crossCheckStateCount[4]++;
      j++;
    }
    if (crossCheckStateCount[4] >= maxCount) {
      return -1;
    }

    // If we found a finder-pattern-like section, but its size is significantly different than
    // the original, assume it's a false positive
    int stateCountTotal = crossCheckStateCount[0] + crossCheckStateCount[1] + crossCheckStateCount[2] + crossCheckStateCount[3] +
        crossCheckStateCount[4];
    if (5 * std::abs(stateCountTotal - originalStateCountTotal) >= originalStateCountTotal) {
      return -1;
    }

    return foundPatternCross(crossCheckStateCount) ? centerFromEnd(crossCheckStateCount, j) : -1;
}

bool FinderPatternFinder::crossCheckDiagonal(int startI, int centerJ, int maxCount, int originalStateCountTotal)
{
    crossCheckStateCount[0]=0;
    crossCheckStateCount[1]=0;
    crossCheckStateCount[2]=0;
    crossCheckStateCount[3]=0;
    crossCheckStateCount[4]=0;

        // Start counting up, left from center finding black center mass
        int i = 0;
        while (startI >= i && centerJ >= i && src.at<uchar>(startI-i,centerJ-i)==0) {
          crossCheckStateCount[2]++;
          i++;
        }

        if (startI < i || centerJ < i) {
          return false;
        }

        // Continue up, left finding white space
        while (startI >= i && centerJ >= i && src.at<uchar>(startI-i,centerJ-i)>0 &&
               crossCheckStateCount[1] <= maxCount) {
          crossCheckStateCount[1]++;
          i++;
        }

        // If already too many modules in this state or ran off the edge:
        if (startI < i || centerJ < i || crossCheckStateCount[1] > maxCount) {
          return false;
        }

        // Continue up, left finding black border
        while (startI >= i && centerJ >= i && src.at<uchar>(startI-i,centerJ-i)==0 &&
               crossCheckStateCount[0] <= maxCount) {
          crossCheckStateCount[0]++;
          i++;
        }
        if (crossCheckStateCount[0] > maxCount) {
           return false;
        }

        int maxI = src.rows;
        int maxJ = src.cols;

        // Now also count down, right from center
        i = 1;
        while (startI + i < maxI && centerJ + i < maxJ && src.at<uchar>(startI+i,centerJ+i)==0) {
          crossCheckStateCount[2]++;
          i++;
        }

        // Ran off the edge?
        if (startI + i >= maxI || centerJ + i >= maxJ) {
           return false;
        }

        while (startI + i < maxI && centerJ + i < maxJ && src.at<uchar>(startI+i,centerJ+i)>0 &&
               crossCheckStateCount[3] < maxCount) {
          crossCheckStateCount[3]++;
          i++;
        }

        if (startI + i >= maxI || centerJ + i >= maxJ || crossCheckStateCount[3] >= maxCount) {
          return false;
        }

        while (startI + i < maxI && centerJ + i < maxJ && src.at<uchar>(startI+i,centerJ+i)==0 &&
               crossCheckStateCount[4] < maxCount) {
          crossCheckStateCount[4]++;
          i++;
        }

        if (crossCheckStateCount[4] >= maxCount) {
          return false;
       }

        // If we found a finder-pattern-like section, but its size is more than 100% different than
        // the original, assume it's a false positive
        int stateCountTotal = crossCheckStateCount[0] + crossCheckStateCount[1] + crossCheckStateCount[2] + crossCheckStateCount[3] + crossCheckStateCount[4];
        return
            std::abs(stateCountTotal - originalStateCountTotal) < 2 * originalStateCountTotal &&
            foundPatternCross(crossCheckStateCount);
}
