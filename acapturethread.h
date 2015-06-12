#ifndef ACAPTURETHREAD_H
#define ACAPTURETHREAD_H

#include <QtCore/QThread>
#include <QtCore/QMutex>

class ACaptureThread : public QThread {
    Q_OBJECT

    signals:
        void captured(const QImage &img);
        void failed();

    public:
        //! Constrcutor.
        explicit ACaptureThread(QObject *parent = NULL);

        //! Destructor.
        virtual ~ACaptureThread() {}

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
        QString _dev_name, _grp_name;

        QMutex _mutex;

};

#endif
