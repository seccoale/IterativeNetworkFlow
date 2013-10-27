#-------------------------------------------------
#
# Project created by QtCreator 2013-10-18T12:45:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IterativeNetworkFlow
TEMPLATE = app


SOURCES += main.cpp\
        graphview.cpp \
    jobdeclarationwindow.cpp

HEADERS  += graphview.h \
    Structs.h \
    jobdeclarationwindow.h

FORMS    += graphview.ui \
    jobdeclarationwindow.ui
