#include "Detector.h"
#include <QDebug>
#include <vector>
#include <cmath>
#include <limits>
#include "finderpatternfinder.h"
Detector::Detector()
{
    testParam = new testparamset;
    initMat();
}

cv::Mat Detector::testAction(cv::Mat& src)
{
    cv::Mat backg;
//    if(testParam->do1){

    cv::Mat imagePoint = getImagePoint(src);


    //test pointToWorld here

 //   cv::Vec2f worldPoint = calWorldPoint(imagePoint);
 //  qDebug()<<"worldPoint is:"<<worldPoint[0]<<", "<<worldPoint[1];

        cv::Vec4i rowLineAve,colLineAve;
        std::vector<cv::Vec4i> lines;

        rowLineAve[1]=rowLineAve[3]=AveRow;
        rowLineAve[0]=0;
        rowLineAve[2]=src.cols-1;

        colLineAve[0]=colLineAve[2]=AveCol;
        colLineAve[1]=0;
        colLineAve[3]=src.rows-1;

        lines.clear();
        lines.push_back(rowLineAve);
        lines.push_back(colLineAve);
        backg = drawLines(src,lines,cv::Scalar(0,0,255));
//    }
    return backg;
}

void Detector::initMat()
{


    // get values from matlab
    float rotateParams[] = {0.9988,0.0158,-0.0456,-0.0174,0.9992,
                           -0.0347,0.0450,0.0355,0.9984};
    float intricParams[] = {5129,0,0,0,5131.3,0,1032.2,
                           848.7977,1};
    float transParams[] = {-6.5986,-36.8060,629.6464};

    rotateMat = cv::Mat(3,3,CV_32FC1,rotateParams);
    intricMat = cv::Mat(3,3,CV_32FC1,intricParams);
    transMat = cv::Mat(1,3,CV_32FC1,transParams);

    qDebug()<<rotateMat.at<float>(1,1);

  //  intricMat = intricMat.inv();
}

cv::Mat Detector::getImagePoint(cv::Mat &src)
{
    cv::Mat graySrc;
    if(src.channels()>1){
       cv::cvtColor(src,graySrc,CV_BGR2GRAY); 
    }else{
        graySrc = src;
    }
    graySrc.convertTo(graySrc,CV_8UC1);

    cv::Mat binarySrc;

    int blockSize = src.rows/8;
    if(blockSize%2==0){
        blockSize++;
    }
    cv::adaptiveThreshold(graySrc,binarySrc,255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY,blockSize,10);

 //   cv::medianBlur(binarySrc, binarySrc, 5);
    float centerRow = centerRecognize(binarySrc,true);
    float centerCol = centerRecognize(binarySrc,false);

//    qDebug()<<"centerRow: "<<centerRow;
//    qDebug()<<"centerCol: "<<centerCol;

//    int count = cv::countNonZero(graySrc);
//    qDebug()<<count;
//    Q_ASSERT_X(centerRow > 0, "error", "no remark found");
//    Q_ASSERT_X(centerCol > 0, "error", "no remark found");

    cv::Mat imPoint(1,3,CV_32FC1);
    imPoint.at<float>(0,0)= centerCol;
    imPoint.at<float>(0,1)= centerRow;
    imPoint.at<float>(0,2)= 1.0;


    if(src.channels()>1){
        src = binarySrc;
        cv::cvtColor(src,src,CV_GRAY2BGR);
    }else{
        src = binarySrc;
    }

    return imPoint;
}

float Detector::centerRecognize(cv::Mat &src, bool rowORcol)
{
    FinderPatternFinder finder(src);
    finder.find();

    QList<FinderPattern> pattern = finder.possibleCenters;

    int centerCount=0;
    int bestOne=0;
    int centerRow=0;
    int centerCol=0;
    if(pattern.isEmpty()){
        qDebug()<<"nothing found";
    }else{
        for(int i=0;i<pattern.size();i++){
            if(pattern[i].count>centerCount){
               bestOne=i;
               centerCount = pattern[i].count;
            }
        }
        centerRow=int(pattern[bestOne].getY());
        centerCol=int(pattern[bestOne].getX());
    }

    AveRow = centerRow;
    AveCol = centerCol;

    if(rowORcol){
        return centerRow;
    }else{
        return centerCol;
    }
    return 0;

}

cv::Vec2f Detector::calWorldPoint(cv::Mat &imagePointWith1)
{
    cv::Mat A = imagePointWith1.cross(intricMat);// 1*3

    cv::Mat B = cv::Mat(3,3,CV_32FC1);
    B.at<float>(2,0) = A.at<float>(0,0);//  0 0 0
    B.at<float>(2,1) = A.at<float>(0,1);//  0 0 0
    B.at<float>(2,2) = A.at<float>(0,2);//    A

    cv::Mat C = cv::Mat(3,3,CV_32FC1);  //  1 0 0
    C.at<float>(0,0) = 1;               //  0 1 0
    C.at<float>(1,1) = 1;               //  0 0 0

    cv::Mat D = transMat.cross((C.cross(rotateMat) + B).inv());

    cv::Vec2f worldPoint;
    worldPoint[0] = D.at<float>(0,0);
    worldPoint[1] = D.at<float>(0,1);

    return worldPoint;
}

cv::Mat Detector::drawLines(cv::Mat& backg, std::vector<cv::Vec4i>& lines,cv::Scalar color)
{
   // cv::Mat temp;
        int width = (backg.rows+backg.cols)/2/350;
        for( size_t i = 0; i < lines.size(); i++ )
        {
          cv::Vec4i l = lines[i];
          cv::line( backg, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), color, width, CV_AA);
        }
    return backg;
}

