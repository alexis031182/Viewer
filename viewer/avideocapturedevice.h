#ifndef AVIDEOCAPTUREDEVICE_H
#define AVIDEOCAPTUREDEVICE_H

#include <QtCore/QSemaphore>

#include "acapturedevice.h"

struct AVFormatContext;
struct SwsContext;
struct AVFrame;

class AVideoCaptureDevice : public ACaptureDevice {
    Q_OBJECT

    public:
        //! Constructor.
        explicit AVideoCaptureDevice(QObject *parent = NULL);

        //! Destructor.
        virtual ~AVideoCaptureDevice() {}

        //! Get is running.
        virtual bool isRunning() const;

        //! Get device name.
        QString deviceName() const;

        //! Set device name.
        void setDeviceName(const QString &name);

        //! Get device group name.
        QString groupName() const;

        //! Set device group name.
        void setGroupName(const QString &name);

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

        AVFormatContext *_av_fmt_ctx;

        SwsContext *_av_sws_ctx;

        AVFrame *_av_cap_frm, *_av_vid_frm;

        int _vid_strm_idx;

        QString _dev_name, _grp_name;

        QSemaphore _semaphore;

        //! Clean.
        void clean();

    private slots:
        //! Thread run.
        void threadRun();

};

#endif
