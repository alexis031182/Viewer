#include <QtCore/QAbstractItemModel>

#include <QtGui/QContextMenuEvent>

#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLayout>
#include <QtWidgets/QMenu>

#include "widgets/amodelselector.h"
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
    layout()->setMargin(0);
    layout()->setSpacing(0);
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
    if(_dev_ctrl) _dev_ctrl->unsetImageWidget();

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

            const QString dev_dsp_name = dev_ctrl->identifier().displayName();
            if(!cur_dsp_name.isEmpty() && dev_dsp_name == cur_dsp_name) {
                dev_action->setCheckable(true);
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
        selector->setTitle(ADeviceView::tr("Enter URL or select video file:"));

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

    QAction *model_select_action = new QAction(&menu);
    model_select_action->setText(ADeviceView::tr("Add new device..."));
    connect(model_select_action, &QAction::triggered, [this]() {
        QDialog dlg(this);
        dlg.setWindowTitle(ADeviceView::tr("New device"));
        dlg.setLayout(new QVBoxLayout());

        AModelSelector *selector = new AModelSelector(&dlg);
        selector->setTitle(ADeviceView::tr("Select video device:"));
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

        if(_dev_ctrl && _dev_ctrl->isCapturing())
            _dev_ctrl->stop();

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

        ADeviceController *dev_ctrl = new ADeviceController(this);
        dev_ctrl->setIdentifier(identifier);
        setController(dev_ctrl);

        AServiceController::instance()->registerDevice(dev_ctrl);

        QMetaObject::invokeMethod(dev_ctrl, "start", Qt::QueuedConnection);
    });

    menu.addAction(model_select_action);

    QAbstractItemModel *vid_flt_model
        = AServiceController::instance()->videoFilterModel();

    if(vid_flt_model && vid_flt_model->rowCount()) {
        menu.addSeparator();

        for(int i = 0, n = vid_flt_model->rowCount(); i < n; ++i) {
            QModelIndex grp_idx = vid_flt_model->index(i,0);
            if(!grp_idx.isValid()) continue;

            QAction *flt_action = new QAction(&menu);
            flt_action->setText(vid_flt_model->data(grp_idx).toString());

            if(vid_flt_model->rowCount(grp_idx)) {
                QMenu *grp_menu = new QMenu(&menu);
                for(int ii = 0, nn = vid_flt_model->rowCount(grp_idx)
                    ; ii < nn; ++ii) {

                    QModelIndex name_idx = vid_flt_model->index(ii, 0, grp_idx);
                    if(!name_idx.isValid()) continue;

                    QModelIndex file_idx = vid_flt_model->index(ii, 1, grp_idx);
                    if(!file_idx.isValid()) continue;

                    QAction *action = new QAction(grp_menu);
                    action->setText(vid_flt_model->data(name_idx).toString());
                    action->setData(vid_flt_model->data(file_idx));
                    connect(action, &QAction::triggered, [this,action]() {
                        if(_dev_ctrl) {
                            _dev_ctrl->setFilter(action->data().toString());
                        }
                    });

                    grp_menu->addAction(action);
                }

                flt_action->setMenu(grp_menu);
            }

            menu.addAction(flt_action);
        }
    }

    QAction *window_remove_action = new QAction(&menu);
    window_remove_action->setText(ADeviceView::tr("Remove window"));
    connect(window_remove_action, &QAction::triggered
        , this, &ADeviceView::deleteLater);

    menu.addSeparator();
    menu.addAction(window_remove_action);
    menu.exec(event->globalPos());

    event->accept();
}
