#-------------------------------------------------
#
# Project created by QtCreator 2016-03-03T17:53:15
#
#-------------------------------------------------

QT       += core gui

QT += network concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cameraLoc
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    imageformat.cpp \
    settingsdialog.cpp \
    testparamset.cpp \
    Detector.cpp \
    finderpattern.cpp \
    finderpatternfinder.cpp

HEADERS  += mainwindow.h \
    imageformat.h \
    settingsdialog.h \
    testparamset.h \
    Detector.h \
    finderpattern.h \
    finderpatternfinder.h

FORMS    += mainwindow.ui \
    settingsdialog.ui \
    testparamset.ui

INCLUDEPATH += E:\opencv\opencv3.0\opencv_file\include\
               E:\opencv\opencv3.0\opencv_file\include\opencv\
               E:\opencv\opencv3.0\opencv_file\include\opencv2
LIBS += E:\opencv\opencv3.0\opencv_file\lib\libopencv_*.a

DISTFILES +=

RESOURCES += \
    images.qrc
