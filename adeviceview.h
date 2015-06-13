#ifndef ADEVICEVIEW_H
#define ADEVICEVIEW_H

#include <QtWidgets/QWidget>

class ADeviceView : public QWidget {
    Q_OBJECT

    public:
        //! Constructor.
        explicit ADeviceView(QWidget *parent = NULL);

        //! Destructor.
        virtual ~ADeviceView() {}

};

#endif
