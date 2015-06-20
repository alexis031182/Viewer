TARGET = butterworthfilter

TEMPLATE = lib

CONFIG += c++11

DEFINES += BUTTERWORTHFILTER_LIBRARY

INCLUDEPATH += ../..

SOURCES += abutterworthfilter.cpp

HEADERS += abutterworthfilter.h \
    butterworthfilter_global.h

unix {
    CONFIG += link_pkgconfig

    PKGCONFIG += opencv
}

DISTFILES += \
    metadata.json
