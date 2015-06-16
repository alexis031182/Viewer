#ifndef AGRAYFILTER_H
#define AGRAYFILTER_H

#include <QtCore/QObject>

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

        //! Get properties dialog.
        virtual QDialog *properties() const;

        //! Run.
        virtual void run(cv::Mat &matrix);

};

#endif
