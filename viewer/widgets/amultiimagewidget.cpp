#include <QtGui/QResizeEvent>

#include "amultiimagewidget.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
AMultiImageWidget::AMultiImageWidget(QWidget *parent)
    : AImageWidget(parent), _preview_switched(false) {

    _preview_img_wdg = new AImageWidget(this);
    _preview_img_wdg->setFramed(true);
    _preview_img_wdg->setFixedSize(160,120);
    _preview_img_wdg->setVisible(false);
    _preview_img_wdg->installEventFilter(this);

    QMetaObject::invokeMethod(this, "updatePreviewPosition"
        , Qt::QueuedConnection);
}


// ========================================================================== //
// Get image.
// ========================================================================== //
QImage AMultiImageWidget::image() const {
    if(_preview_switched)
        return _preview_img_wdg->image();

    return AImageWidget::image();
}


// ========================================================================== //
// Get preview image.
// ========================================================================== //
QImage AMultiImageWidget::previewImage() const {
    if(_preview_switched)
        return AImageWidget::image();

    return _preview_img_wdg->image();
}


// ========================================================================== //
// Set image.
// ========================================================================== //
void AMultiImageWidget::setImage(const QImage &img) {
    switch(_preview_switched) {
        case true: _preview_img_wdg->setImage(img); break;
        case false: AImageWidget::setImage(img); break;
    }
}


// ========================================================================== //
// Set preview image.
// ========================================================================== //
void AMultiImageWidget::setPreviewImage(const QImage &img) {
    switch(_preview_switched) {
        case true: AImageWidget::setImage(img); break;
        case false: _preview_img_wdg->setImage(img); break;
    }
}


// ========================================================================== //
// Show preview.
// ========================================================================== //
void AMultiImageWidget::showPreview() {_preview_img_wdg->show();}


// ========================================================================== //
// Hide preview.
// ========================================================================== //
void AMultiImageWidget::hidePreview() {
    _preview_switched = false; _preview_img_wdg->hide();
}


// ========================================================================== //
// Switch preview.
// ========================================================================== //
void AMultiImageWidget::switchPreview() {
    _preview_switched = !_preview_switched;
}


// ========================================================================== //
// Resize event.
// ========================================================================== //
void AMultiImageWidget::resizeEvent(QResizeEvent *event) {
    AImageWidget::resizeEvent(event); updatePreviewPosition();
}


// ========================================================================== //
// Event filter.
// ========================================================================== //
bool AMultiImageWidget::eventFilter(QObject *obj, QEvent *event) {
    if(_preview_img_wdg == obj) {
        if(event->type() == QEvent::Enter)
            _preview_img_wdg->setCursor(Qt::PointingHandCursor);
        else if(event->type() == QEvent::Leave)
            _preview_img_wdg->unsetCursor();
        else if(event->type() == QEvent::MouseButtonRelease)
            switchPreview();
    }

    return QWidget::eventFilter(obj, event);
}


// ========================================================================== //
// Update preview position.
// ========================================================================== //
void AMultiImageWidget::updatePreviewPosition() {
    const int x = this->width() - _preview_img_wdg->width() - 4;
    const int y = 4;

    _preview_img_wdg->move(x, y);
}
