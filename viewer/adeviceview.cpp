#include <QtCore/QAbstractItemModel>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QTimer>

#include <QtGui/QResizeEvent>

#include <QtWidgets/QGraphicsOpacityEffect>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMenu>

#include "widgets/aimagewidget.h"

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

    _flt_menu = new QMenu(this);
    connect(_flt_menu, &QMenu::aboutToShow, [this]() {
        _action_wdg_animate_freezed = true;
    });
    connect(_flt_menu, &QMenu::aboutToHide, [this]() {
        _action_wdg_animate_freezed = false;
    });

    _url_act_grp = new QActionGroup(this);
    _dev_act_grp = new QActionGroup(this);
    _flt_act_grp = new QActionGroup(this);

    foreach(ADeviceController *ctrl
        , AServiceController::instance()->devices())
        createDeviceAction(ctrl);

    connect(AServiceController::instance()
        , &AServiceController::deviceRegistered
        , this, &ADeviceView::createDeviceAction);

    QAbstractItemModel *flt_model
        = AServiceController::instance()->videoFilterModel();

    if(flt_model) {
        QAction *flt_disabled_action = new QAction(_flt_act_grp);
        flt_disabled_action->setText(ADeviceView::tr("Disabled"));
        flt_disabled_action->setCheckable(true);
        flt_disabled_action->setChecked(true);
        connect(flt_disabled_action, &QAction::triggered, [this]() {
            if(_dev_ctrl) _dev_ctrl->unsetFilter();
        });

        _flt_menu->addAction(flt_disabled_action);

        for(int i = 0, n = flt_model->rowCount(); i < n; ++i)
            createFilterGroupAction(i);
    }

    setLayout(new QVBoxLayout());
    layout()->setMargin(0);
    layout()->setSpacing(0);
    layout()->addWidget(_img_wdg);

    createTitleWidget();
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

    if(_dev_ctrl) {
        _dev_ctrl->setImageWidget(_img_wdg);

        _title_label->setText(_dev_ctrl->identifier().displayName());

        animateShowTitleWidget();
    }
}


// ========================================================================== //
// Resize event.
// ========================================================================== //
void ADeviceView::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    _title_label->move(0,0);
    _title_label->resize(event->size().width(), _title_label->height());

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
    QAction *action
        = new QAction((ctrl->identifier().hasValue(ADeviceIdentifier::TYPE_URL))
            ? _url_act_grp : _dev_act_grp);

    action->setText(ctrl->identifier().displayName());
    action->setCheckable(true);
    connect(action, &QAction::triggered, [this,ctrl]() {
        if(_dev_ctrl) {
            if(_dev_ctrl == ctrl) return;
            if(_dev_ctrl->isCapturing()) _dev_ctrl->stop();
        }

        setController(ctrl);

        QMetaObject::invokeMethod(ctrl, "start", Qt::QueuedConnection);
    });

    if(ctrl->identifier().hasValue(ADeviceIdentifier::TYPE_URL)) {
        _url_menu->addAction(action);

    } else {
        _dev_menu->addAction(action);
    }
}


// ========================================================================== //
// Create filter group action.
// ========================================================================== //
void ADeviceView::createFilterGroupAction(int grp_i) {
    QAbstractItemModel *model
        = AServiceController::instance()->videoFilterModel();

    QModelIndex grp_idx = model->index(grp_i,0);
    if(!grp_idx.isValid()) return;

    QAction *grp_action = new QAction(_flt_act_grp);
    grp_action->setText(model->data(grp_idx).toString());

    if(model->rowCount(grp_idx)) {
        QMenu *grp_menu = new QMenu(_flt_menu);
        for(int i = 0, n = model->rowCount(grp_idx); i < n; ++i) {
            QModelIndex name_idx = model->index(i, 0, grp_idx);
            if(!name_idx.isValid()) continue;

            QModelIndex file_idx = model->index(i, 1, grp_idx);
            if(!file_idx.isValid()) continue;

            const QString name = model->data(name_idx).toString();
            const QString file = model->data(file_idx).toString();

            QAction *action = new QAction(name, _flt_act_grp);
            action->setCheckable(true);
            connect(action, &QAction::triggered, [this,name,file]() {
                if(!_dev_ctrl) return;

                if(_dev_ctrl->filter() != file)
                    _dev_ctrl->setFilter(file);

                QWidget *flt_wdg = _dev_ctrl->filterProperties();
                if(flt_wdg) {
                    QDialog dlg(this);
                    dlg.setWindowTitle(name);
                    dlg.setLayout(new QVBoxLayout());

                    flt_wdg->setParent(&dlg);

                    dlg.layout()->addWidget(flt_wdg);

                    QDialogButtonBox *btn_box = new QDialogButtonBox(&dlg);
                    btn_box->setStandardButtons(QDialogButtonBox::Ok);

                    connect(btn_box, &QDialogButtonBox::accepted
                        , &dlg, &QDialog::accept);
                    connect(btn_box, &QDialogButtonBox::rejected
                        , &dlg, &QDialog::reject);

                    dlg.layout()->addWidget(btn_box);
                    dlg.exec();
                }
            });

            grp_menu->addAction(action);
        }

        grp_action->setMenu(grp_menu);
    }

    _flt_menu->addAction(grp_action);
}


// ========================================================================== //
// Create title widget.
// ========================================================================== //
void ADeviceView::createTitleWidget() {
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(1.0);

    _title_label = new QLabel(this);
    _title_label->setMinimumHeight(50);
    _title_label->setGraphicsEffect(effect);
    _title_label->setAlignment(Qt::AlignTop|Qt::AlignLeft);
    _title_label->setMargin(8);
}


// ========================================================================== //
// Animate title widget.
// ========================================================================== //
void ADeviceView::animateTitleWidget(bool showed) {
    QGraphicsOpacityEffect *effect
        = qobject_cast<QGraphicsOpacityEffect*>(_title_label->graphicsEffect());

    if(!effect) return;

    QPropertyAnimation *animation = new QPropertyAnimation(_title_label);
    animation->setTargetObject(effect);
    animation->setPropertyName("opacity");
    animation->setDuration(1000);
    animation->setStartValue(effect->opacity());

    if(showed) {
        animation->setEndValue(1.0);

        QTimer *timer = new QTimer(_title_label);
        timer->setSingleShot(true);
        timer->setInterval(5000);

        connect(animation, &QPropertyAnimation::finished
            , timer, static_cast<void(QTimer::*)()>(&QTimer::start));
        connect(timer, &QTimer::timeout
            , this, &ADeviceView::animateHideTitleWidget);
        connect(timer, &QTimer::timeout
            , timer, &QTimer::deleteLater);

    } else animation->setEndValue(0.0);

    connect(animation, &QPropertyAnimation::finished
        , animation, &QPropertyAnimation::deleteLater);

    animation->start();
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

    QToolButton *filter_tbut = new QToolButton(_action_wdg);
    filter_tbut->setIcon(QIcon(QStringLiteral(":/images/filter.png")));
    filter_tbut->setIconSize(QSize(24,24));
    filter_tbut->setToolTip(ADeviceView::tr("Select filter"));
    filter_tbut->setFocusPolicy(Qt::NoFocus);
    filter_tbut->setAutoRaise(true);
    filter_tbut->setPopupMode(QToolButton::InstantPopup);
    filter_tbut->setMenu(_flt_menu);

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

    QToolButton *close_tbut = new QToolButton(_action_wdg);
    close_tbut->setIcon(QIcon(QStringLiteral(":/images/reject.png")));
    close_tbut->setIconSize(QSize(24,24));
    close_tbut->setToolTip(ADeviceView::tr("Close"));
    close_tbut->setFocusPolicy(Qt::NoFocus);
    close_tbut->setAutoRaise(true);
    connect(close_tbut, &QToolButton::clicked, this, &ADeviceView::deleteLater);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setSpacing(0);
    layout->setMargin(4);
    layout->addWidget(resource_tbut);
    layout->addWidget(device_tbut);
    layout->addWidget(filter_tbut);
    layout->addStretch(1);
    layout->addWidget(start_tbut);
    layout->addWidget(stop_tbut);
    layout->addStretch(1);
    layout->addWidget(close_tbut);

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
// Animate show title widget.
// ========================================================================== //
void ADeviceView::animateShowTitleWidget() {animateTitleWidget(true);}


// ========================================================================== //
// Animate hide title widget.
// ========================================================================== //
void ADeviceView::animateHideTitleWidget() {animateTitleWidget(false);}


// ========================================================================== //
// Animate show action widget.
// ========================================================================== //
void ADeviceView::animateShowActionWidget() {animateActionWidget(true);}


// ========================================================================== //
// Animate hide action widget.
// ========================================================================== //
void ADeviceView::animateHideActionWidget() {animateActionWidget(false);}
