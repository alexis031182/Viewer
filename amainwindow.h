#ifndef AMAINWINDOW_H
#define AMAINWINDOW_H

#include <QtWidgets/QMainWindow>

class AMainWindow : public QMainWindow {
    Q_OBJECT

    public:
        //! Constructor.
        explicit AMainWindow(QWidget *parent = NULL);

        //! Destructor.
        virtual ~AMainWindow() {}

};

#endif
