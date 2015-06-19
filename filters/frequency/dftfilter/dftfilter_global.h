#ifndef DFTFILTER_GLOBAL_H
#define DFTFILTER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DFTFILTER_LIBRARY)
#  define DFTFILTERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DFTFILTERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif
