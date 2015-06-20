TEMPLATE = subdirs

SUBDIRS += general/grayfilter \
    general/blurfilter \
    frequency/dftfilter \
    general/sharpfilter \
    frequency/butterworthfilter

HEADERS += afilterinterface.h
