#include <QtWidgets/QApplication>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QLayout>

#include "amainwindow.h"
#include "aservicecontroller.h"
#include "adeviceview.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
AMainWindow::AMainWindow(QWidget *parent) : QMainWindow(parent) {
    setCentralWidget(new QWidget(this));
    centralWidget()->setLayout(new QGridLayout());
    centralWidget()->layout()->setMargin(2);
    centralWidget()->layout()->setSpacing(2);

    createActions(); createMenus(); createToolBars(); statusBar();
}


// ========================================================================== //
// Create actions.
// ========================================================================== //
void AMainWindow::createActions() {
    _file_quit_action = new QAction(this);
    _file_quit_action->setText(AMainWindow::tr("Quit"));
    connect(_file_quit_action, &QAction::triggered, qApp, &QApplication::quit);

    _window_add_action = new QAction(this);
    _window_add_action->setText(AMainWindow::tr("Add"));
    _window_add_action->setIcon(QIcon(QStringLiteral(
        ":/images/window-add.png")));

    connect(_window_add_action, &QAction::triggered, [this]() {
        new ADeviceView(centralWidget()); updateViewLayout();
    });
}


// ========================================================================== //
// Create menus.
// ========================================================================== //
void AMainWindow::createMenus() {
    QMenu *file_menu = menuBar()->addMenu(AMainWindow::tr("File"));
    file_menu->addAction(_file_quit_action);

    QMenu *window_menu = menuBar()->addMenu(AMainWindow::tr("Window"));
    window_menu->addAction(_window_add_action);
}


// ========================================================================== //
// Create toolbars.
// ========================================================================== //
void AMainWindow::createToolBars() {
    QToolBar *window_toolbar = addToolBar(AMainWindow::tr("Window"));
    window_toolbar->addAction(_window_add_action);
}


// ========================================================================== //
// Update view layout.
// ========================================================================== //
void AMainWindow::updateViewLayout() {
    QGridLayout *layout = static_cast<QGridLayout*>(centralWidget()->layout());

    QLayoutItem *item = NULL;
    while((item = layout->takeAt(0)) != 0)
        delete item;

    QList<ADeviceView*> views = centralWidget()->findChildren<ADeviceView*>();
    if(!views.isEmpty()) {
        const int n = views.size();
        const int q = (n <= 4) ? 2 : (n <= 6) ? 3 : 4;

        QListIterator<ADeviceView*> itr(views);
        while(itr.hasNext()) {
            int row = layout->rowCount() - 1;
            int col = layout->count() % q;
            if(col == 0) {++row; col = 0;}
            layout->addWidget(itr.next(), row, col);
        }
    }
}
