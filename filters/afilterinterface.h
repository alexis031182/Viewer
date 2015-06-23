#ifndef AFILTERINTERFACE
#define AFILTERINTERFACE

#include <opencv2/core.hpp>

#include <QtCore/QSharedPointer>
#include <QtCore/QtPlugin>

class QString;
class QDialog;

class AFilterInterface {
    public:
        //! Destructor.
        virtual ~AFilterInterface() {}

        //! Get instance copy.
        virtual QSharedPointer<AFilterInterface> copy() const = 0;

        //! Get display group.
        virtual QString displayGroup() const = 0;

        //! Get display name.
        virtual QString displayName() const = 0;

        //! Get properties widget.
        virtual QWidget *properties() = 0;

        //! Run.
        virtual void run(cv::Mat &matrix) = 0;

};

#define AFilterInterface_iid "ru.AlexisSoft.Viewer.AFilterInterface.1.0"
Q_DECLARE_INTERFACE(AFilterInterface, AFilterInterface_iid)

#endif

