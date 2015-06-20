TARGET = sharpfilter

TEMPLATE = lib

CONFIG += c++11

DEFINES += SHARPFILTER_LIBRARY

INCLUDEPATH += ../..

SOURCES += asharpfilter.cpp

HEADERS += asharpfilter.h \
    sharpfilter_global.h

unix {
    CONFIG += link_pkgconfig

    PKGCONFIG += opencv
}

DISTFILES += \
    metadata.json
