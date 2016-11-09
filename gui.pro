#-------------------------------------------------
#
# Project created by QtCreator 2016-09-01T09:27:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gui
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    logindlg.cpp

HEADERS  += mainwindow.h \
    logindlg.h

FORMS    += mainwindow.ui \
    logindlg.ui

RESOURCES += \
    resource.qrc
