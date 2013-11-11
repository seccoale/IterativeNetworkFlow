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
    taskdeclarationwindow.cpp \
    infalgoithms.cpp \
    fordfulkersonrunner.cpp

HEADERS  += graphview.h \
    Structs.h \
    taskdeclarationwindow.h \
    infalgoithms.h \
    fordfulkersonrunner.h

FORMS    += graphview.ui \
    taskdeclarationwindow.ui
