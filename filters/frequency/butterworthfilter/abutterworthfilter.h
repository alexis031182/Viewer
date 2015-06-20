#ifndef ABUTTERWORTHFILTER_H
#define ABUTTERWORTHFILTER_H

#include <QtCore/QObject>
#include <QtCore/QMutex>

#include "butterworthfilter_global.h"
#include "afilterinterface.h"

class BUTTERWORTHFILTERSHARED_EXPORT AButterworthFilter
    : public QObject, public AFilterInterface {

    Q_OBJECT
    Q_PLUGIN_METADATA(IID AFilterInterface_iid FILE "metadata.json")
    Q_INTERFACES(AFilterInterface)

    public:
        //! Constructor.
        explicit AButterworthFilter();

        //! Destructor.
        virtual ~AButterworthFilter() {}

        //! Get display group.
        virtual QString displayGroup() const;

        //! Get display name.
        virtual QString displayName() const;

        //! Get properties widget.
        virtual QWidget *properties();

        //! Run.
        virtual void run(cv::Mat &matrix);

    private:
        enum Type {TYPE_LOWPASS, TYPE_HIGHPASS};

        Type _type;

        int _order, _radius;

        cv::Mat _krn_mat;

        QMutex _mutex;

        //! Create kernel.
        void createKernel(const cv::Size &size, Type type
            , int order, int radius, cv::Mat &mat) const;

        //! Shift dft.
        void shiftDft(cv::Mat &mat) const;

};

#endif
