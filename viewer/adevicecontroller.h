#ifndef ADEVICECONTROLLER_H
#define ADEVICECONTROLLER_H

#include <QtCore/QObject>

#include "adeviceidentifier.h"

class ACaptureDevice;
class AFilterDevice;

class ADeviceController : public QObject {
    Q_OBJECT

    signals:
        void starting();
        void started();

        void stopping();
        void stopped();

        void captured(const QImage &img);
        void filtered(const QImage &img);

        void captureFpsChanged(double fps);
        void previewFpsChanged(double fps);

        void failed();

    public:
        //! Constructor.
        explicit ADeviceController(QObject *parent = NULL);

        //! Destructor.
        virtual ~ADeviceController() {}

        //! Get device identifier.
        const ADeviceIdentifier &identifier() const;

        //! Set device identifier.
        void setIdentifier(const ADeviceIdentifier &identifier);

        //! Get filter.
        QString filter() const;

        //! Set filter.
        void setFilter(const QString &fname);

        //! Unset filter.
        void unsetFilter();

        //! Get filter properties.
        QWidget *filterProperties() const;

        //! Get is capturing.
        bool isCapturing() const;

    public slots:
        //! Start.
        void start();

        //! Stop.
        void stop();

    private:
        ADeviceIdentifier _identifier;

        ACaptureDevice *_cap_dev;

        AFilterDevice *_flt_dev;

};

#endif
