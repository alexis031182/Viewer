#include <QtCore/QAbstractItemModel>

#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLayout>

#include "widgets/amodelselector.h"
#include "widgets/aurlselector.h"

#include "amainwindow.h"
#include "aservicecontroller.h"
#include "adevicecontroller.h"
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
    _file_url_action = new QAction(this);
    _file_url_action->setText(AMainWindow::tr("Add new resource..."));
    _file_url_action->setIcon(QIcon(QStringLiteral(":/images/network.png")));
    connect(_file_url_action, &QAction::triggered, [this]() {
        QDialog dlg(this);
        dlg.setWindowTitle(AMainWindow::tr("New resource"));
        dlg.setLayout(new QVBoxLayout());

        AUrlSelector *selector = new AUrlSelector(&dlg);
        selector->setTitle(AMainWindow::tr("Enter URL or select video file:"));

        dlg.layout()->addWidget(selector);

        QDialogButtonBox *btn_box = new QDialogButtonBox(&dlg);
        btn_box->setStandardButtons(QDialogButtonBox::Ok
            | QDialogButtonBox::Cancel);

        connect(btn_box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btn_box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

        dlg.layout()->addWidget(btn_box);
        if(dlg.exec() != QDialog::Accepted) return;

        ADeviceIdentifier identifier;
        identifier.setValue(ADeviceIdentifier::TYPE_URL, selector->url());

        ADeviceController *ctrl = new ADeviceController(this);
        ctrl->setIdentifier(identifier);

        AServiceController::instance()->registerDevice(ctrl);

        statusBar()->showMessage(AMainWindow::tr("Resource appended:")
            + QLatin1Char(' ') + identifier.displayName(), 5000);
    });

    _file_dev_action = new QAction(this);
    _file_dev_action->setText(AMainWindow::tr("Add new device..."));
    _file_dev_action->setIcon(QIcon(QStringLiteral(":/images/camera.png")));
    connect(_file_dev_action, &QAction::triggered, [this]() {
        QDialog dlg(this);
        dlg.setWindowTitle(AMainWindow::tr("New device"));
        dlg.setLayout(new QVBoxLayout());

        AModelSelector *selector = new AModelSelector(&dlg);
        selector->setTitle(AMainWindow::tr("Select video device:"));
        selector->setModel(AServiceController::instance()->videoDeviceModel());

        dlg.layout()->addWidget(selector);

        QDialogButtonBox *btn_box = new QDialogButtonBox(&dlg);
        btn_box->setStandardButtons(QDialogButtonBox::Ok
            | QDialogButtonBox::Cancel);

        connect(btn_box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btn_box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

        dlg.layout()->addWidget(btn_box);
        if(dlg.exec() != QDialog::Accepted) return;

        QModelIndex index = selector->currentIndex();
        if(!index.isValid()) return;

        if(index.column() != 0) {
            index = selector->model()->index(index.row(), 0, index.parent());
            if(!index.isValid()) return;
        }

        ADeviceIdentifier identifier;
        switch(index.parent().isValid()) {
            case true:
                identifier.setValue(ADeviceIdentifier::TYPE_DEV
                    , selector->model()->data(index));
                identifier.setValue(ADeviceIdentifier::TYPE_GRP
                    , selector->model()->data(index.parent()));
            break;

            case false:
                identifier.setValue(ADeviceIdentifier::TYPE_GRP
                    , selector->model()->data(index));
            break;
        }

        ADeviceController *ctrl = new ADeviceController(this);
        ctrl->setIdentifier(identifier);

        AServiceController::instance()->registerDevice(ctrl);

        statusBar()->showMessage(AMainWindow::tr("Device appended:")
            + QLatin1Char(' ') + identifier.displayName(), 5000);
    });

    _file_quit_action = new QAction(this);
    _file_quit_action->setText(AMainWindow::tr("Quit"));
    connect(_file_quit_action, &QAction::triggered, qApp, &QApplication::quit);

    _window_add_action = new QAction(this);
    _window_add_action->setText(AMainWindow::tr("Add"));
    _window_add_action->setIcon(QIcon(QStringLiteral(
        ":/images/window-add.png")));

    connect(_window_add_action, &QAction::triggered, [this]() {
        ADeviceView *dev_view = new ADeviceView(centralWidget());
        connect(dev_view, &QObject::destroyed
            , this, &AMainWindow::updateViewLayout
            , Qt::QueuedConnection);

        updateViewLayout();
    });
}


// ========================================================================== //
// Create menus.
// ========================================================================== //
void AMainWindow::createMenus() {
    QMenu *file_menu = menuBar()->addMenu(AMainWindow::tr("File"));
    file_menu->addAction(_file_url_action);
    file_menu->addAction(_file_dev_action);
    file_menu->addSeparator();
    file_menu->addAction(_file_quit_action);

    QMenu *window_menu = menuBar()->addMenu(AMainWindow::tr("Window"));
    window_menu->addAction(_window_add_action);
}


// ========================================================================== //
// Create toolbars.
// ========================================================================== //
void AMainWindow::createToolBars() {
    QToolBar *file_toolbar = addToolBar(AMainWindow::tr("File"));
    file_toolbar->addAction(_file_url_action);
    file_toolbar->addAction(_file_dev_action);

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
