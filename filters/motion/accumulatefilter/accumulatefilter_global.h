#ifndef ACCUMULATEFILTER_GLOBAL_H
#define ACCUMULATEFILTER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ACCUMULATEFILTER_LIBRARY)
#  define ACCUMULATEFILTERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ACCUMULATEFILTERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif
