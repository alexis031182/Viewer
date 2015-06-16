#ifndef AFILTERINTERFACE
#define AFILTERINTERFACE

#include <opencv2/core.hpp>

#include <QtCore/QtPlugin>

class QString;
class QDialog;

class AFilterInterface {
    public:
        //! Destructor.
        virtual ~AFilterInterface() {}

        //! Get display group.
        virtual QString displayGroup() const = 0;

        //! Get display name.
        virtual QString displayName() const = 0;

        //! Get properties dialog.
        virtual QDialog *properties() const = 0;

        //! Run.
        virtual void run(cv::Mat &matrix) = 0;

};

#define AFilterInterface_iid "ru.AlexisSoft.Viewer.AFilterInterface.1.0"
Q_DECLARE_INTERFACE(AFilterInterface, AFilterInterface_iid)

#endif

