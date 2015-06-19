TARGET = dftfilter

TEMPLATE = lib

CONFIG += c++11

DEFINES += DFTFILTER_LIBRARY

INCLUDEPATH += ../..

SOURCES += adftfilter.cpp

HEADERS += adftfilter.h \
    dftfilter_global.h

unix {
    CONFIG += link_pkgconfig

    PKGCONFIG += opencv
}

DISTFILES += \
    metadata.json
