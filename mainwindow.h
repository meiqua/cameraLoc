#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>

#include <qtcore>
#include <qfiledialog>
#include <qmainwindow>
#include <qpushbutton>
#include <qlabel>
#include <qimage>
#include <qpixmap>
#include <qslider>
#include <qhboxlayout>
#include <qapplication>
#include <qkeyevent>
#include <algorithm>
#include <vector>
#include <QtGui>
#include <qwidget>
#include <iostream>
#include <QMap>
#include <QByteArray>

#include <opencv2/video/background_segm.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <testparamset.h>
#include <ImageFormat.h>

#include "Detector.h"
QT_BEGIN_NAMESPACE

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class SettingsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    /** Load video to process.
    */
    void actionOpen();

    void actionClose();

    /** Main loop to process each frame.
    */
    void timerLoop();

    void savePictures();

    void receiveRefresh();

    void startMyTimer();
    void stopMyTimer();

protected:
    void resizeEvent(QResizeEvent *event);



private:
    bool isDigitStr(QString src);

    cv::Mat loadFromQrc(QString qrc);

    void SetScreen(QString qrc);
    QString imageOnScreen;

    QLabel *msgLabel;
    Ui::MainWindow *ui;
    SettingsDialog *settings;

  //  TestParamSet* testParam;

    QLabel *videoFrameLabel;
    QLabel *frameEdgeLabel;

    cv::Mat rawFrame,rawCopyFrame,frameForEdgeDetect,edge;
    QImage  frameImage;
    QTimer *timer;
    cv::VideoCapture videoCapture;
    bool openSuccess;
    Detector* myDetector;
};

#endif // MAINWINDOW_H
