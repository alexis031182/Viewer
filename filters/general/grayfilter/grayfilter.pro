TARGET = grayfilter

TEMPLATE = lib

CONFIG += c++11

DEFINES += GRAYFILTER_LIBRARY

INCLUDEPATH += ../..

SOURCES += agrayfilter.cpp

HEADERS += agrayfilter.h \
    grayfilter_global.h

unix {
    CONFIG += link_pkgconfig

    PKGCONFIG += opencv
}

DISTFILES += \
    metadata.json
