#include <QtGui/QContextMenuEvent>

#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLayout>
#include <QtWidgets/QMenu>

#include "widgets/aimagewidget.h"
#include "widgets/aurlselector.h"

#include "adeviceview.h"
#include "aservicecontroller.h"
#include "adevicecontroller.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
ADeviceView::ADeviceView(QWidget *parent) : QWidget(parent) {
    _img_wdg = new AImageWidget(this);

    setLayout(new QVBoxLayout());
    layout()->setMargin(4);
    layout()->setSpacing(4);
    layout()->addWidget(_img_wdg);
}


// ========================================================================== //
// Get device controller.
// ========================================================================== //
ADeviceController *ADeviceView::controller() const {return _dev_ctrl;}


// ========================================================================== //
// Set device controller.
// ========================================================================== //
void ADeviceView::setController(ADeviceController *ctrl) {
    if(_dev_ctrl) {
        _dev_ctrl->unsetImageWidget();
        _img_wdg->setImage(QImage());
    }

    _dev_ctrl = ctrl;

    if(_dev_ctrl) _dev_ctrl->setImageWidget(_img_wdg);
}


// ========================================================================== //
// Context menu event.
// ========================================================================== //
void ADeviceView::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu(this);

    QList<ADeviceController*> devices
        = AServiceController::instance()->devices();

    if(!devices.isEmpty()) {
        const QString cur_dsp_name
            = (_dev_ctrl) ? _dev_ctrl->identifier().displayName() : QString();

        QListIterator<ADeviceController*> itr(devices);
        while(itr.hasNext()) {
            ADeviceController *dev_ctrl = itr.next();

            QAction *dev_action = new QAction(&menu);
            dev_action->setCheckable(true);

            const QString dev_dsp_name = dev_ctrl->identifier().displayName();
            if(!cur_dsp_name.isEmpty() && dev_dsp_name == cur_dsp_name) {
                dev_action->setChecked(true);

            } else {
                connect(dev_action, &QAction::triggered, [this,dev_ctrl]() {
                    if(_dev_ctrl && _dev_ctrl->isCapturing())
                        _dev_ctrl->stop();

                    setController(dev_ctrl);

                    QMetaObject::invokeMethod(dev_ctrl, "start"
                        , Qt::QueuedConnection);
                });
            }

            dev_action->setText(dev_dsp_name);

            menu.addAction(dev_action);
        }

        menu.addSeparator();
    }

    QAction *url_select_action = new QAction(&menu);
    url_select_action->setText(ADeviceView::tr("Add new resource..."));
    connect(url_select_action, &QAction::triggered, [this]() {
        QDialog dlg(this);
        dlg.setWindowTitle(ADeviceView::tr("New resource"));
        dlg.setLayout(new QVBoxLayout());

        AUrlSelector *selector = new AUrlSelector(&dlg);
        selector->setTitle(ADeviceView::tr("Enter URL or select a video file"));

        dlg.layout()->addWidget(selector);

        QDialogButtonBox *btn_box = new QDialogButtonBox(&dlg);
        btn_box->setStandardButtons(QDialogButtonBox::Ok
            | QDialogButtonBox::Cancel);

        connect(btn_box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btn_box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

        dlg.layout()->addWidget(btn_box);
        if(dlg.exec() != QDialog::Accepted) return;

        if(_dev_ctrl && _dev_ctrl->isCapturing())
            _dev_ctrl->stop();

        ADeviceIdentifier identifier;
        identifier.setValue(ADeviceIdentifier::TYPE_URL, selector->url());

        ADeviceController *dev_ctrl = new ADeviceController(this);
        dev_ctrl->setIdentifier(identifier);
        setController(dev_ctrl);

        AServiceController::instance()->registerDevice(dev_ctrl);

        QMetaObject::invokeMethod(dev_ctrl, "start", Qt::QueuedConnection);
    });

    menu.addAction(url_select_action);
    menu.exec(event->globalPos());

    event->accept();
}
