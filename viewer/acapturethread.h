#ifndef ACAPTURETHREAD_H
#define ACAPTURETHREAD_H

#include <QtCore/QThread>
#include <QtCore/QMutex>

class AFilterInterface;

class ACaptureThread : public QThread {
    Q_OBJECT

    signals:
        void captured(const QImage &img);

        void captureFpsChanged(double fps);
        void previewFpsChanged(double fps);

        void failed();

    public:
        //! Constrcutor.
        explicit ACaptureThread(QObject *parent = NULL);

        //! Destructor.
        virtual ~ACaptureThread() {}

        //! Get filter.
        AFilterInterface *filter();

        //! Set filter.
        void setFilter(AFilterInterface *filter);

        //! Unset filter.
        void unsetFilter();

        //! Get capture fps.
        double captureFps();

        //! Get preview fps.
        double previewFps();

        //! Get device name.
        QString deviceName();

        //! Get device group name.
        QString groupName();

    public slots:
        //! Set device name.
        void setDeviceName(const QString &name);

        //! Set device group name.
        void setGroupName(const QString &name);

    protected:
        //! Run.
        virtual void run();

    private:
        AFilterInterface *_filter;

        double _vid_strm_cap_avg_fps, _vid_strm_prw_avg_fps;

        QString _dev_name, _grp_name;

        QMutex _mutex;

        //! Set capture fps.
        void setCaptureFps(double fps);

        //! Set preview fps.
        void setPreviewFps(double fps);

};

#endif
