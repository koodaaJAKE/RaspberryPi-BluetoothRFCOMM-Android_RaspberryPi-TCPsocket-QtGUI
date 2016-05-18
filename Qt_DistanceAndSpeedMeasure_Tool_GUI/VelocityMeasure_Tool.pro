#-------------------------------------------------
#
# Project created by QtCreator 2015-12-06T16:51:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VelocityMeasure_Tool
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    TCP_SocketClient.cpp

HEADERS  += mainwindow.h \
    TCP_SocketClient.h

FORMS    += mainwindow.ui
