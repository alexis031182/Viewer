#ifndef ADEVICECONTROLLER_H
#define ADEVICECONTROLLER_H

#include <QtCore/QPointer>
#include <QtCore/QObject>

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

        //! Get image widget.
        AImageWidget *imageWidget() const;

        //! Set image widget.
        void setImageWidget(AImageWidget *wdg);

    public slots:
        //! Start.
        void start();

        //! Stop.
        void stop();

    private:
        ACaptureThread *_capture;

        QPointer<AImageWidget> _img_wdg;

};

#endif
