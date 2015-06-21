#ifndef ACAPTUREDEVICE_H
#define ACAPTUREDEVICE_H

#include "adevice.h"

class ACaptureDevice : public ADevice {
    Q_OBJECT

    signals:
        void captureFpsChanged(double fps);

    public:
        //! Constructor.
        explicit ACaptureDevice(QObject *parent = NULL);

        //! Destructor.
        virtual ~ACaptureDevice() {}

};

#endif
