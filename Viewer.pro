QT += core gui widgets

TARGET = Viewer

TEMPLATE = app

CONFIG += c++11

DEFINES += __STDC_CONSTANT_MACROS

SOURCES += main.cpp \
    amainwindow.cpp

HEADERS += amainwindow.h

unix {
    CONFIG += link_pkgconfig

    PKGCONFIG += libavdevice libavutil libavformat libavcodec libswscale
}
