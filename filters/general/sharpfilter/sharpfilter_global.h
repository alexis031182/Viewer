#ifndef SHARPFILTER_GLOBAL_H
#define SHARPFILTER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SHARPFILTER_LIBRARY)
#  define SHARPFILTERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define SHARPFILTERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif
