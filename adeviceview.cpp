#include <QtWidgets/QLayout>

#include "widgets/aimagewidget.h"

#include "adeviceview.h"
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
