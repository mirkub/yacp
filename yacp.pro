#-------------------------------------------------
#
# Project created by QtCreator 2015-11-27T22:41:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = yacp
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    communicator.cpp \
    configuration.cpp

HEADERS  += mainwindow.h \
    communicator.h \
    configuration.h

FORMS    += mainwindow.ui \
    configuration.ui

unix|win32: LIBS += -lzyre

unix|win32: LIBS += -lzmq

unix|win32: LIBS += -lczmq
