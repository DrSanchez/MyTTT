#-------------------------------------------------
#
# Project created by QtCreator 2015-05-14T00:17:50
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = myttt-server
CONFIG   += console
CONFIG   -= app_bundl

# Adds Cx11 support, e.g. nullptr...
CONFIG   += c++11 (Qt5)

TEMPLATE = app


SOURCES += main.cpp \
    tttserver.cpp \
    gamemanager.cpp \
    globalupdatethread.cpp

HEADERS += \
    tttserver.h \
    gamemanager.h \
    globalupdatethread.h


INCLUDEPATH += $${_PRO_FILE_PWD_}/../myttt-common/
