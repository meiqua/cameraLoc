#ifndef Detector_H
#define Detector_H
#include <opencv2/video/background_segm.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <testparamset.h>

class Detector:public QObject
{
    Q_OBJECT
public:
    Detector();

    cv::Mat drawLines(cv::Mat& backg, std::vector<cv::Vec4i>& lines,cv::Scalar color);

    cv::Mat testAction(cv::Mat& src);

    testparamset* testParam;

    cv::Mat rotateMat,transMat,intricMat;
    void initMat();
    cv::Mat getImagePoint(cv::Mat& src);
    float centerRecognize(cv::Mat& src,bool rowORcol);
    cv::Vec2f calWorldPoint(cv::Mat& imagePointWith1);
    int startRowToDisplay,endRowToDisplay,startColToDisplay,endColToDisplay,AveRow,AveCol;
    std::vector<int> rowsTo,colsTo;
};

#endif // Detector_H
