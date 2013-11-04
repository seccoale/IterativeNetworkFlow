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
    infalgoithms.cpp

HEADERS  += graphview.h \
    Structs.h \
    taskdeclarationwindow.h \
    infalgoithms.h

FORMS    += graphview.ui \
    taskdeclarationwindow.ui
