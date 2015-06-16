#ifndef ASERVICECONTROLLER_H
#define ASERVICECONTROLLER_H

#include <QtCore/QPointer>
#include <QtCore/QObject>

class QStandardItemModel;
class QAbstractItemModel;

class ADeviceController;

class AServiceController : public QObject {
    Q_OBJECT

    public:
        //! Get instance.
        static AServiceController *instance();

        //! Constructor.
        explicit AServiceController(QObject *parent = NULL);

        //! Destructor.
        virtual ~AServiceController() {}

        //! Register device.
        void registerDevice(ADeviceController *ctrl);

        //! Get devices.
        QList<ADeviceController*> devices() const;

        //! Get video device model.
        QAbstractItemModel *videoDeviceModel() const;

    public slots:
        //! Update video device model.
        void updateVideoDeviceModel();

    private:
        QObject *_devices_obj;

        QPointer<QStandardItemModel> _vid_dev_model;

        //! Create video device model.
        void createVideoDeviceModel();

};

#endif
