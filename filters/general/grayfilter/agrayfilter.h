#ifndef AGRAYFILTER_H
#define AGRAYFILTER_H

#include <QtCore/QObject>
#include <QtCore/QMutex>

#include "grayfilter_global.h"
#include "afilterinterface.h"

class GRAYFILTERSHARED_EXPORT AGrayFilter
    : public QObject, public AFilterInterface {

    Q_OBJECT
    Q_PLUGIN_METADATA(IID AFilterInterface_iid FILE "metadata.json")
    Q_INTERFACES(AFilterInterface)

    public:
        //! Constructor.
        explicit AGrayFilter();

        //! Destructor.
        virtual ~AGrayFilter() {}

        //! Get display group.
        virtual QString displayGroup() const;

        //! Get display name.
        virtual QString displayName() const;

        //! Get properties widget.
        virtual QWidget *properties();

        //! Run.
        virtual void run(cv::Mat &matrix);

    private:
        enum Mode {
            MODE_GRAY,
            MODE_RGB_R,
            MODE_RGB_G,
            MODE_RGB_B,
            MODE_HSV_H,
            MODE_HSV_S,
            MODE_HSV_V
        };

        Mode _mode;

        QMutex _mutex;

};

#endif
