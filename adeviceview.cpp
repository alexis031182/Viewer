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
        QListIterator<ADeviceController*> itr(devices);
        while(itr.hasNext()) {
            QAction *dev_action = new QAction(&menu);

            ADeviceController *dev_ctrl = itr.next();

            const bool has_url
                = dev_ctrl->identifier().hasValue(ADeviceIdentifier::TYPE_URL);
            const bool has_dev
                = dev_ctrl->identifier().hasValue(ADeviceIdentifier::TYPE_DEV);
            const bool has_grp
                = dev_ctrl->identifier().hasValue(ADeviceIdentifier::TYPE_GRP);

            if(has_url) {
                dev_action->setText(dev_ctrl->identifier()
                    .value(ADeviceIdentifier::TYPE_URL).toString());

            } else {
                if(has_dev && has_grp) {
                    const QString dev
                        = dev_ctrl->identifier()
                            .value(ADeviceIdentifier::TYPE_DEV).toString();

                    const QString grp
                        = dev_ctrl->identifier()
                            .value(ADeviceIdentifier::TYPE_GRP).toString();

                    dev_action->setText(grp + QLatin1Char('/') + dev);

                } else if(has_dev) {
                    dev_action->setText(dev_ctrl->identifier()
                        .value(ADeviceIdentifier::TYPE_DEV).toString());

                } else if(has_grp) {
                    dev_action->setText(dev_ctrl->identifier()
                        .value(ADeviceIdentifier::TYPE_GRP).toString());

                } else dev_action->setText(ADeviceView::tr("Unknown"));
            }

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
