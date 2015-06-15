QT += core gui widgets

TARGET = Viewer

TEMPLATE = app

CONFIG += c++11

DEFINES += __STDC_CONSTANT_MACROS

SOURCES += main.cpp \
    amainwindow.cpp \
    aservicecontroller.cpp \
    widgets/aimagewidget.cpp \
    acapturethread.cpp \
    adevicecontroller.cpp \
    adeviceview.cpp \
    widgets/aurlselector.cpp \
    widgets/amodelselector.cpp \
    adeviceidentifier.cpp

HEADERS += amainwindow.h \
    aservicecontroller.h \
    widgets/aimagewidget.h \
    acapturethread.h \
    adevicecontroller.h \
    adeviceview.h \
    widgets/aurlselector.h \
    widgets/amodelselector.h \
    adeviceidentifier.h \
    filters/afilterinterface.h

unix {
    CONFIG += link_pkgconfig

    PKGCONFIG += opencv libavdevice libavutil libavformat libavcodec libswscale
}

RESOURCES += images.qrc
