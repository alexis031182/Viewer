TARGET = grayfilter

TEMPLATE = lib

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
