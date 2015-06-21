QT += core gui widgets

TARGET = viewer

TEMPLATE = app

CONFIG += c++11

DEFINES += __STDC_CONSTANT_MACROS

INCLUDEPATH += ../filters

SOURCES += main.cpp \
    amainwindow.cpp \
    aservicecontroller.cpp \
    widgets/aimagewidget.cpp \
    adevicecontroller.cpp \
    adeviceview.cpp \
    widgets/aurlselector.cpp \
    widgets/amodelselector.cpp \
    adeviceidentifier.cpp \
    widgets/amultiimagewidget.cpp \
    adevice.cpp \
    afilterdevice.cpp \
    acapturedevice.cpp \
    aimagecapturedevice.cpp \
    avideocapturedevice.cpp

HEADERS += amainwindow.h \
    aservicecontroller.h \
    widgets/aimagewidget.h \
    adevicecontroller.h \
    adeviceview.h \
    widgets/aurlselector.h \
    widgets/amodelselector.h \
    adeviceidentifier.h \
    widgets/amultiimagewidget.h \
    adevice.h \
    afilterdevice.h \
    acapturedevice.h \
    aimagecapturedevice.h \
    avideocapturedevice.h

unix {
    CONFIG += link_pkgconfig

    PKGCONFIG += opencv libavdevice libavutil libavformat libavcodec libswscale
}

RESOURCES += images.qrc
