#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "settingsdialog.h"
#include <QDir>
#include <QKeyEvent>
#include <QtNetwork>
#include <qtconcurrentrun.h>
#include <QFileDialog>
#include<QMessageBox>
#include <QSplitter>


//save picture and videocapture should move to thread

using namespace std;

// Main GUI
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {

     ui->setupUi(this);

     myDetector = new Detector;

     settings = new SettingsDialog;
 //    testParam = new TestParamSet;

     videoFrameLabel =ui->labelLeft;
     frameEdgeLabel =ui->labelRight;

     QHBoxLayout *layout = new QHBoxLayout;
     layout->addWidget(videoFrameLabel);
     layout->addWidget(frameEdgeLabel);

    centralWidget()->setLayout(layout);

     openSuccess = false;

     timer = new QTimer(this);

    // Connect slots
    connect(timer, SIGNAL(timeout()), this, SLOT(timerLoop()));

    connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(actionOpen()));
    connect(ui->actionClose,SIGNAL(triggered()),this,SLOT(actionClose()));
    connect(ui->actionConfigure, SIGNAL(triggered()), settings, SLOT(show()));
    connect(ui->actionCapture,SIGNAL(triggered()),this,SLOT(savePictures()));

    connect(ui->actionStart,SIGNAL(triggered()),this,SLOT(startMyTimer()));
    connect(ui->actionStop,SIGNAL(triggered()),this,SLOT(stopMyTimer()));

    connect(ui->actionTestParamSet, SIGNAL(triggered()), myDetector->testParam, SLOT(show()));
    connect(myDetector->testParam,SIGNAL(refresh()),this,SLOT(receiveRefresh()));

    // Set initial screen
 //    SetScreen(":/images/initialScreen.jpg");
        SetScreen(":/images/test4.jpg");
//     qDebug()<<videoFrameLabel->size().width();
}

// Load video to process.
void MainWindow::actionOpen() {

    // Load video and start video
    if (!timer->isActive()) {
        // Open file dialog
   //     QString fileName = QFileDialog::getOpenFileName(this, "Open Video", ".", "Video Files (*.avi *.mpg)");
   //       videoCapture.open(fileName.toStdString().c_str());//fileName.toAscii().data());

      SettingsDialog::Settings p = settings->settings();
      QString address = p.IPaddress;

      if(isDigitStr(address))
       videoCapture.open(CV_CAP_DSHOW + address.toInt()); //open webcam on laptop when type in 0
      else
      {
           ui->statusBar->showMessage("opening....",2000);

            videoCapture.open((address.toStdString().c_str()));
            //This capture is blocking,unfortunately I have not found a unblocking solution yet.
         qDebug()<<"open ip address :" << videoCapture.isOpened() << endl;
      }


      if(videoCapture.isOpened())
      {
          ui->statusBar->showMessage("opening camera success",3000);

          // Get frame per second
          int fps=videoCapture.get(CV_CAP_PROP_FPS);
          qDebug()<<"fps is: "<<fps<<endl;
          //   qDebug("fps: %d", fps);

          // Set timer interval
          if(fps>0)
          timer->setInterval(1000/fps);
          else
           timer->setInterval(1000/10);  //set 10 as default fps

          timer->start();

           ui->actionOpen->setEnabled(false);
           ui->actionClose->setEnabled(true);
      }
      else
      {
          ui->statusBar->showMessage("open camera fail",3000);
          openSuccess = false;
          SetScreen(":/images/failScreen.jpg");
      }


    } else {
        timer->stop();
        ui->statusBar->showMessage("open camera fail",3000);
        openSuccess = false;
        SetScreen(":/images/failScreen.jpg");
    }
}

void MainWindow::actionClose()
{

    ui->actionOpen->setEnabled(true);
    ui->actionClose->setEnabled(false);

    if(videoCapture.isOpened())
        videoCapture.release();

    if(timer->isActive())
        timer->stop();

    SetScreen(":/images/closeScreen.jpg");
    openSuccess = false;
}

// Main loop activated with the timer.
void MainWindow::timerLoop() {

    // Capture frame from video
    videoCapture>>rawFrame;
  //  qDebug()<<"totalFrame is:"<<totalFrame;

    // Check if frame was successfully captured.
    if (rawFrame.empty()) {
        timer->stop();
        ui->statusBar->showMessage("the remote camera send back empty frame",3000);
        SetScreen(":/images/failScreen.jpg");
        openSuccess = false;
    } else {
        rawCopyFrame = rawFrame.clone();  //save as picture
        frameForEdgeDetect = rawFrame.clone();
        edge = myDetector->testAction(frameForEdgeDetect);
        int w ;
        int h ;
        int w2 ;
        int h2 ;
        w = videoFrameLabel->width();
        h = videoFrameLabel->height();

        w2 = frameEdgeLabel->width();
        h2 = frameEdgeLabel->height();

        cv::resize(rawFrame, rawFrame, cv::Size(rawFrame.size().width,rawFrame.size().height), 0, 0, CV_INTER_LINEAR);
        videoFrameLabel->setPixmap(QPixmap::fromImage(ImageFormat::Mat2QImage(rawFrame)).scaled(w,h,Qt::KeepAspectRatio));

        cv::resize(edge, edge, cv::Size(edge.size().width,edge.size().height), 0, 0, CV_INTER_LINEAR);
        frameEdgeLabel->setPixmap(QPixmap::fromImage(ImageFormat::Mat2QImage(edge)).scaled(w2,h2,Qt::KeepAspectRatio));

        openSuccess = true;
    }

}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    if(!openSuccess)
   SetScreen(imageOnScreen);

}

MainWindow::~MainWindow()
{
    delete settings;
    delete ui;
}

bool MainWindow::isDigitStr(QString src)
{
    QByteArray ba = src.toLatin1();// convert QString to char*
     const char *s = ba.data();

    while(*s && *s>='0' && *s<='9') s++;

    if (*s)
    { //not a number
        return false;
    }
    else
    { //number
        return true;
    }
}

void MainWindow::savePictures()
{
      if (!rawCopyFrame.empty())
      {
          QImage img = ImageFormat::Mat2QImage(rawCopyFrame);
          QString Path;
          if(settings->settings().picturePath == "")
          {
              Path = QFileDialog::getExistingDirectory(this, tr("Save images"),
                                                              settings->settings().picturePath,
                                                              QFileDialog::ShowDirsOnly
                                                              | QFileDialog::DontResolveSymlinks);
               settings->updatePath(Path);
          }

             Path = settings->settings().picturePath;

             QDir dir(Path);
             dir.setFilter(QDir::Files | QDir::NoSymLinks);
             dir.setSorting(QDir::Name | QDir::Reversed);
             QFileInfoList list = dir.entryInfoList();

             int fileNum = list.size()+1;
             Path = Path+QDir::separator()+QString::number(fileNum,10)+".png";
           //  qDebug() << "path is: " << list.size() <<endl;

             if(Path.isEmpty())
             {
                 return;
             }
             else
             {

                if(! (img.save(Path) ) ) //save
                  {
                      QMessageBox::information(this, tr("Failed to save the image"),
                                               tr("Failed to save the image!"));
                       return;
                  }
             }
      }

}

void MainWindow::receiveRefresh()
{
    QResizeEvent *event;
    resizeEvent(event);
}

void MainWindow::startMyTimer()
{
    if (!timer->isActive())
    timer->start();
}

void MainWindow::stopMyTimer()
{
    if (timer->isActive())
        timer->stop();
}

cv::Mat MainWindow::loadFromQrc(QString qrc)
{
    //double tic = double(getTickCount());

    QFile file(qrc);
    cv::Mat m;
    if(file.open(QIODevice::ReadOnly))
    {
        qint64 sz = file.size();
        std::vector<uchar> buf(sz);
        file.read((char*)buf.data(), sz);
        m = cv::imdecode(buf, cv::IMREAD_COLOR); //
    }
    //double toc = (double(getTickCount()) - tic) * 1000.0 / getTickFrequency();
    //qDebug() << "OpenCV loading time: " << toc;

    return m;
}

void MainWindow::SetScreen(QString qrc)
{
    cv::Mat src = loadFromQrc(qrc);
    cv::Mat edge = src.clone();

    QImage tempImage = ImageFormat::Mat2QImage(src);
    int w = videoFrameLabel->width();
    int h = videoFrameLabel->height();
    videoFrameLabel->setPixmap(QPixmap::fromImage(tempImage).scaled(w,h,Qt::KeepAspectRatio));

    w = frameEdgeLabel->width();
    h = frameEdgeLabel->height();
    frameEdgeLabel->setPixmap(QPixmap::fromImage(ImageFormat::Mat2QImage(myDetector->testAction(edge))).scaled(w,h,Qt::KeepAspectRatio));

    imageOnScreen = qrc;

//    qDebug()<<videoFrameLabel->size().width();
}

