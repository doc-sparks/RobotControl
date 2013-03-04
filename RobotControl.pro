#-------------------------------------------------
#
# Project created by QtCreator 2013-01-18T21:40:11
#
#-------------------------------------------------

QT       += core gui

TARGET = RobotControl
TEMPLATE = app

LIBS += -Lpython2.7 -lpython2.7

SOURCES += main.cpp\
        mainwindow.cpp \
    nxtcontrolthread.cpp

HEADERS  += mainwindow.h \
    nxtcontrolthread.h

FORMS    += mainwindow.ui
