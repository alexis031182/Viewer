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

        //! Get video filter model.
        QAbstractItemModel *videoFilterModel() const;

    public slots:
        //! Update video device model.
        void updateVideoDeviceModel();

        //! Update video filter model.
        void updateVideoFilterModel();

    private:
        QObject *_devices_obj;

        QPointer<QStandardItemModel> _vid_dev_model, _vid_flt_model;

        //! Create video device model.
        void createVideoDeviceModel();

        //! Create video filter model.
        void createVideoFilterModel();

};

#endif
