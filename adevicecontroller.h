#ifndef ADEVICECONTROLLER_H
#define ADEVICECONTROLLER_H

#include <QtCore/QPointer>
#include <QtCore/QObject>

#include "adeviceidentifier.h"

class ACaptureThread;
class AImageWidget;

class ADeviceController : public QObject {
    Q_OBJECT

    signals:
        void starting();
        void started();

        void stopping();
        void stopped();

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

        //! Get is capturing.
        bool isCapturing() const;

        //! Get image widget.
        AImageWidget *imageWidget() const;

        //! Set image widget.
        void setImageWidget(AImageWidget *wdg);

        //! Unset image widget.
        void unsetImageWidget();

    public slots:
        //! Start.
        void start();

        //! Stop.
        void stop();

    private:
        ADeviceIdentifier _identifier;

        ACaptureThread *_capture;

        QPointer<AImageWidget> _img_wdg;

};

#endif
