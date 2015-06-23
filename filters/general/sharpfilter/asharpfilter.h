#ifndef ASHARPFILTER_H
#define ASHARPFILTER_H

#include <QtCore/QObject>
#include <QtCore/QMutex>

#include "sharpfilter_global.h"
#include "afilterinterface.h"

class SHARPFILTERSHARED_EXPORT ASharpFilter
    : public QObject, public AFilterInterface {

    Q_OBJECT
    Q_PLUGIN_METADATA(IID AFilterInterface_iid FILE "metadata.json")
    Q_INTERFACES(AFilterInterface)

    public:
        //! Constructor.
        explicit ASharpFilter();

        //! Destructor.
        virtual ~ASharpFilter() {}

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
        QMutex _mutex;

};

#endif
