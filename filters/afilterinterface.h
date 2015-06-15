#ifndef AFILTERINTERFACE
#define AFILTERINTERFACE

#include <opencv2/core.hpp>

#include <QtCore/QtPlugin>

class QWidget;

class AFilterInterface {
    public:
        //! Destructor.
        virtual ~AFilterInterface() {}

        //! Use.
        virtual void use(QWidget *parent = NULL) = 0;

        //! Run.
        virtual void run(cv::Mat &matrix) = 0;

};

#define AFilterInterface_iid "ru.AlexisSoft.Viewer.AFilterInterface.1.0"
Q_DECLARE_INTERFACE(AFilterInterface, AFilterInterface_iid)

#endif

