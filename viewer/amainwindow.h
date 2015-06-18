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

    private:
        QAction *_file_url_action, *_file_dev_action, *_file_quit_action;

        QAction *_window_add_action;

        //! Create actions.
        void createActions();

        //! Create menus.
        void createMenus();

        //! Create toolbars.
        void createToolBars();

    private slots:
        //! Update view layout.
        void updateViewLayout();

};

#endif
