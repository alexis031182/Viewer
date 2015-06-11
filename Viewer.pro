QT += core gui widgets

TARGET = Viewer

TEMPLATE = app

CONFIG += c++11

DEFINES += __STDC_CONSTANT_MACROS

SOURCES += main.cpp \
    amainwindow.cpp \
    widgets/asourceselector.cpp \
    aservicecontroller.cpp

HEADERS += amainwindow.h \
    widgets/asourceselector.h \
    aservicecontroller.h

unix {
    CONFIG += link_pkgconfig

    PKGCONFIG += libavdevice libavutil libavformat libavcodec libswscale
}
