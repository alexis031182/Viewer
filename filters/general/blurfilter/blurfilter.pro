TARGET = blurfilter

TEMPLATE = lib

CONFIG += c++11

DEFINES += BLURFILTER_LIBRARY

INCLUDEPATH += ../..

SOURCES += ablurfilter.cpp

HEADERS += ablurfilter.h \
    blurfilter_global.h

unix {
    CONFIG += link_pkgconfig

    PKGCONFIG += opencv
}

DISTFILES += \
    metadata.json
