#ifndef BLURFILTER_GLOBAL_H
#define BLURFILTER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(BLURFILTER_LIBRARY)
#  define BLURFILTERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define BLURFILTERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif
