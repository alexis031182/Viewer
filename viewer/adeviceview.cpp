#include <QtCore/QAbstractItemModel>
#include <QtCore/QPropertyAnimation>

#include <QtGui/QContextMenuEvent>
#include <QtGui/QResizeEvent>

#include <QtWidgets/QGraphicsOpacityEffect>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QToolButton>
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
ADeviceView::ADeviceView(QWidget *parent)
    : QWidget(parent), _action_wdg_animate_freezed(false) {

    _img_wdg = new AImageWidget(this);

    _url_menu = new QMenu(this);
    connect(_url_menu, &QMenu::aboutToShow, [this]() {
        _action_wdg_animate_freezed = true;
    });
    connect(_url_menu, &QMenu::aboutToHide, [this]() {
        _action_wdg_animate_freezed = false;
    });

    _dev_menu = new QMenu(this);
    connect(_dev_menu, &QMenu::aboutToShow, [this]() {
        _action_wdg_animate_freezed = true;
    });
    connect(_dev_menu, &QMenu::aboutToHide, [this]() {
        _action_wdg_animate_freezed = false;
    });

    foreach(ADeviceController *ctrl
        , AServiceController::instance()->devices())
        createDeviceAction(ctrl);

    connect(AServiceController::instance()
        , &AServiceController::deviceRegistered
        , this, &ADeviceView::createDeviceAction);

    setLayout(new QVBoxLayout());
    layout()->setMargin(0);
    layout()->setSpacing(0);
    layout()->addWidget(_img_wdg);

    createActionWidget();
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

        QAction *flt_unset_action = new QAction(&menu);
        flt_unset_action->setText(ADeviceView::tr("Unset"));
        connect(flt_unset_action, &QAction::triggered, [this]() {
            if(_dev_ctrl) _dev_ctrl->unsetFilter();
        });

        menu.addAction(flt_unset_action);

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

                    const QString name
                        = vid_flt_model->data(name_idx).toString();
                    const QString file
                        = vid_flt_model->data(file_idx).toString();

                    QAction *action = new QAction(name, grp_menu);
                    connect(action, &QAction::triggered, [this,name,file]() {
                        if(_dev_ctrl) {
                            if(_dev_ctrl->filter() != file)
                                _dev_ctrl->setFilter(file);

                            QWidget *wdg = _dev_ctrl->filterProperties();
                            if(wdg) {
                                QDialog dlg(this);
                                dlg.setWindowTitle(name);
                                dlg.setLayout(new QVBoxLayout());

                                wdg->setParent(&dlg);

                                dlg.layout()->addWidget(wdg);

                                QDialogButtonBox *btn_box
                                    = new QDialogButtonBox(&dlg);
                                btn_box->setStandardButtons(
                                    QDialogButtonBox::Ok);

                                connect(btn_box, &QDialogButtonBox::accepted
                                    , &dlg, &QDialog::accept);
                                connect(btn_box, &QDialogButtonBox::rejected
                                    , &dlg, &QDialog::reject);

                                dlg.layout()->addWidget(btn_box);
                                dlg.exec();
                            }
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


// ========================================================================== //
// Resize event.
// ========================================================================== //
void ADeviceView::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    _action_wdg->move(0,event->size().height() - _action_wdg->height());
    _action_wdg->resize(event->size().width(), _action_wdg->height());
}


// ========================================================================== //
// Event filter.
// ========================================================================== //
bool ADeviceView::eventFilter(QObject *obj, QEvent *event) {
    if(_action_wdg == obj) {
        if(event->type() == QEvent::Enter) animateShowActionWidget();
        else if(event->type() == QEvent::Leave) animateHideActionWidget();
    }

    return QWidget::eventFilter(obj, event);
}


// ========================================================================== //
// Create device action.
// ========================================================================== //
void ADeviceView::createDeviceAction(ADeviceController *ctrl) {
    QAction *action = new QAction(this);
    action->setText(ctrl->identifier().displayName());
    connect(action, &QAction::triggered, [this,ctrl]() {
        if(_dev_ctrl) {
            if(_dev_ctrl == ctrl) return;
            if(_dev_ctrl->isCapturing()) _dev_ctrl->stop();
        }

        setController(ctrl);

        QMetaObject::invokeMethod(ctrl, "start", Qt::QueuedConnection);
    });

    if(ctrl->identifier().hasValue(ADeviceIdentifier::TYPE_URL)) {
        action->setParent(_url_menu);
        _url_menu->addAction(action);

    } else {
        action->setParent(_dev_menu);
        _dev_menu->addAction(action);
    }
}


// ========================================================================== //
// Create action widget.
// ========================================================================== //
void ADeviceView::createActionWidget() {
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(1.0);

    _action_wdg = new QWidget(this);
    _action_wdg->setMinimumHeight(50);
    _action_wdg->setGraphicsEffect(effect);
    _action_wdg->installEventFilter(this);

    QToolButton *resource_tbut = new QToolButton(_action_wdg);
    resource_tbut->setIcon(QIcon(QStringLiteral(":/images/network.png")));
    resource_tbut->setIconSize(QSize(24,24));
    resource_tbut->setToolTip(ADeviceView::tr("Select resource"));
    resource_tbut->setFocusPolicy(Qt::NoFocus);
    resource_tbut->setAutoRaise(true);
    resource_tbut->setPopupMode(QToolButton::InstantPopup);
    resource_tbut->setMenu(_url_menu);

    QToolButton *device_tbut = new QToolButton(_action_wdg);
    device_tbut->setIcon(QIcon(QStringLiteral(":/images/camera.png")));
    device_tbut->setIconSize(QSize(24,24));
    device_tbut->setToolTip(ADeviceView::tr("Select device"));
    device_tbut->setFocusPolicy(Qt::NoFocus);
    device_tbut->setAutoRaise(true);
    device_tbut->setPopupMode(QToolButton::InstantPopup);
    device_tbut->setMenu(_dev_menu);

    QToolButton *start_tbut = new QToolButton(_action_wdg);
    start_tbut->setIcon(QIcon(QStringLiteral(":/images/start.png")));
    start_tbut->setIconSize(QSize(24,24));
    start_tbut->setToolTip(ADeviceView::tr("Start"));
    start_tbut->setFocusPolicy(Qt::NoFocus);
    start_tbut->setAutoRaise(true);
    connect(start_tbut, &QToolButton::clicked, [this]() {
        if(_dev_ctrl) _dev_ctrl->start();
    });

    QToolButton *stop_tbut = new QToolButton(_action_wdg);
    stop_tbut->setIcon(QIcon(QStringLiteral(":/images/stop.png")));
    stop_tbut->setIconSize(QSize(24,24));
    stop_tbut->setToolTip(ADeviceView::tr("Stop"));
    stop_tbut->setFocusPolicy(Qt::NoFocus);
    stop_tbut->setAutoRaise(true);
    connect(stop_tbut, &QToolButton::clicked, [this]() {
        if(_dev_ctrl) _dev_ctrl->stop();
    });

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setSpacing(0);
    layout->setMargin(4);
    layout->addWidget(resource_tbut);
    layout->addWidget(device_tbut);
    layout->addStretch(1);
    layout->addWidget(start_tbut);
    layout->addWidget(stop_tbut);
    layout->addStretch(1);

    _action_wdg->setLayout(layout);

    QMetaObject::invokeMethod(this, "animateHideActionWidget"
        , Qt::QueuedConnection);
}


// ========================================================================== //
// Animate action widget.
// ========================================================================== //
void ADeviceView::animateActionWidget(bool showed) {
    if(_action_wdg_animate_freezed) return;

    QGraphicsOpacityEffect *effect
        = qobject_cast<QGraphicsOpacityEffect*>(_action_wdg->graphicsEffect());

    if(!effect) return;

    QPropertyAnimation *animation = new QPropertyAnimation(_action_wdg);
    animation->setTargetObject(effect);
    animation->setPropertyName("opacity");
    animation->setDuration(1000);
    animation->setStartValue(effect->opacity());

    if(showed) animation->setEndValue(1.0);
    else animation->setEndValue(0.0);

    connect(animation, &QPropertyAnimation::finished
        , animation, &QPropertyAnimation::deleteLater);

    animation->start();
}


// ========================================================================== //
// Animate show action widget.
// ========================================================================== //
void ADeviceView::animateShowActionWidget() {animateActionWidget(true);}


// ========================================================================== //
// Animate hide action widget.
// ========================================================================== //
void ADeviceView::animateHideActionWidget() {animateActionWidget(false);}
