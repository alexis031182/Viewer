#ifndef ADEVICE_H
#define ADEVICE_H

#include <QtCore/QObject>

class ADevice : public QObject {
    Q_OBJECT

    signals:
        void starting();
        void started();

        void stopping();
        void stopped();

        void completed(const QImage &img);

        void failed();

    public:
        //! Constructor.
        explicit ADevice(QObject *parent = NULL);

        //! Destructor.
        virtual ~ADevice() {}

        //! Get is running.
        virtual bool isRunning() const = 0;

    public slots:
        //! Start.
        virtual void start() = 0;

        //! Stop.
        virtual void stop() = 0;

};

#endif
