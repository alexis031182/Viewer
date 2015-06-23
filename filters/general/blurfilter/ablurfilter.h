#ifndef ABLURFILTER_H
#define ABLURFILTER_H

#include <QtCore/QObject>
#include <QtCore/QMutex>

#include "blurfilter_global.h"
#include "afilterinterface.h"

class BLURFILTERSHARED_EXPORT ABlurFilter
    : public QObject, public AFilterInterface {

    Q_OBJECT
    Q_PLUGIN_METADATA(IID AFilterInterface_iid FILE "metadata.json")
    Q_INTERFACES(AFilterInterface)

    public:
        //! Constructor.
        explicit ABlurFilter();

        //! Destructor.
        virtual ~ABlurFilter() {}

        //! Get instance copy.
        virtual QSharedPointer<AFilterInterface> copy() const;

        //! Get display group.
        virtual QString displayGroup() const;

        //! Get display name.
        virtual QString displayName() const;

        //! Get properties widget.
        virtual QWidget *properties();

        //! Run.
        virtual void run(cv::Mat &matrix);

    private:
        enum Type {TYPE_NORMALIZED, TYPE_GAUSSIAN, TYPE_MEDIAN};

        Type _type;

        int _krnl_size;

        QMutex _mutex;

};

#endif
