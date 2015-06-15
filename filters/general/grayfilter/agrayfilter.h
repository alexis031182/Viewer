#ifndef AGRAYFILTER_H
#define AGRAYFILTER_H

#include <QtCore/QObject>

#include "grayfilter_global.h"
#include "afilterinterface.h"

class GRAYFILTERSHARED_EXPORT AGrayFilter
    : public QObject, public AFilterInterface {

    Q_OBJECT
    Q_PLUGIN_METADATA(IID AFilterInterface_iid)
    Q_INTERFACES(AFilterInterface)

    public:
        //! Constructor.
        explicit AGrayFilter();

        //! Destructor.
        virtual ~AGrayFilter() {}

        //! Use.
        virtual void use(QWidget *parent = NULL);

        //! Run.
        virtual void run(cv::Mat &matrix);

};

#endif
