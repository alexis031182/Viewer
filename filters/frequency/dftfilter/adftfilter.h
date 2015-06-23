#ifndef ADFTFILTER_H
#define ADFTFILTER_H

#include <QtCore/QObject>
#include <QtCore/QMutex>

#include "dftfilter_global.h"
#include "afilterinterface.h"

class DFTFILTERSHARED_EXPORT ADftFilter
    : public QObject, public AFilterInterface {

    Q_OBJECT
    Q_PLUGIN_METADATA(IID AFilterInterface_iid FILE "metadata.json")
    Q_INTERFACES(AFilterInterface)

    public:
        //! Constructor.
        explicit ADftFilter();

        //! Destructor.
        virtual ~ADftFilter() {}

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
