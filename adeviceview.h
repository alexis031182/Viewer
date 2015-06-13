#ifndef ADEVICEVIEW_H
#define ADEVICEVIEW_H

#include <QtCore/QPointer>

#include <QtWidgets/QWidget>

class ADeviceController;
class AImageWidget;

class ADeviceView : public QWidget {
    Q_OBJECT

    public:
        //! Constructor.
        explicit ADeviceView(QWidget *parent = NULL);

        //! Destructor.
        virtual ~ADeviceView() {}

        //! Get device controller.
        ADeviceController *controller() const;

        //! Set device controller.
        void setController(ADeviceController *ctrl);

    private:
        AImageWidget *_img_wdg;

        QPointer<ADeviceController> _dev_ctrl;

};

#endif
