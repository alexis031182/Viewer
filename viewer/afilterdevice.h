#ifndef AFILTERDEVICE_H
#define AFILTERDEVICE_H

#include <QtCore/QSemaphore>

#include "adevice.h"

class QPluginLoader;

class AFilterDevice : public ADevice {
    Q_OBJECT

    public:
        //! Constructor.
        explicit AFilterDevice(QObject *parent = NULL);

        //! Constructor.
        explicit AFilterDevice(const QString &fname, QObject *parent = NULL);

        //! Destructor.
        virtual ~AFilterDevice() {}

        //! Get is running.
        virtual bool isRunning() const;

        //! Get file name.
        QString fileName() const;

        //! Set file name.
        void setFileName(const QString &fname);

        //! Get properties.
        QWidget *properties() const;

    public slots:
        //! Start.
        virtual void start();

        //! Stop.
        virtual void stop();

        //! Run.
        virtual void run(const QImage &img);

    private:
        bool _running;

        QPluginLoader *_plugin;

        QSemaphore _semaphore;

    private slots:
        //! Thread run.
        void threadRun(const QImage &src_img);

};

#endif
