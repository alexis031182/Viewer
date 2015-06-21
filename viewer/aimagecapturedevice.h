#ifndef AIMAGECAPTUREDEVICE_H
#define AIMAGECAPTUREDEVICE_H

#include <QtGui/QImage>

#include "acapturedevice.h"

class AImageCaptureDevice : public ACaptureDevice {
    Q_OBJECT

    public:
        //! Constructor.
        explicit AImageCaptureDevice(QObject *parent = NULL);

        //! Constructor.
        explicit AImageCaptureDevice(const QString &fname
            , QObject *parent = NULL);

        //! Destructor.
        virtual ~AImageCaptureDevice() {}

        //! Get is running.
        virtual bool isRunning() const;

        //! Get file name.
        QString fileName() const;

        //! Set file name.
        void setFileName(const QString &fname);

    public slots:
        //! Start.
        virtual void start();

        //! Stop.
        virtual void stop();

    protected:
        //! Timer event.
        virtual void timerEvent(QTimerEvent *event);

    private:
        int _timer_id;

        QString _fname;

        QImage _img;

};

#endif
