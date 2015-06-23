#ifndef AACCUMULATEFILTER_H
#define AACCUMULATEFILTER_H

#include <QtCore/QObject>
#include <QtCore/QMutex>

#include "accumulatefilter_global.h"
#include "afilterinterface.h"

class ACCUMULATEFILTERSHARED_EXPORT AAccumulateFilter
    : public QObject, public AFilterInterface {

    Q_OBJECT
    Q_PLUGIN_METADATA(IID AFilterInterface_iid FILE "metadata.json")
    Q_INTERFACES(AFilterInterface)

    public:
        //! Constructor.
        explicit AAccumulateFilter();

        //! Destructor.
        virtual ~AAccumulateFilter() {}

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
        int _alpha;

        cv::Mat _acc_mat;

        QMutex _mutex;

};

#endif
