TARGET = accumulatefilter

TEMPLATE = lib

CONFIG += c++11

DEFINES += ACCUMULATEFILTER_LIBRARY

INCLUDEPATH += ../..

SOURCES += aaccumulatefilter.cpp \
    asliderwidget.cpp

HEADERS += aaccumulatefilter.h\
        accumulatefilter_global.h \
    asliderwidget.h

unix {
    CONFIG += link_pkgconfig

    PKGCONFIG += opencv
}

DISTFILES += \
    metadata.json
