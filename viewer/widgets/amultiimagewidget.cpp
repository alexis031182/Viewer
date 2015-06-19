#include <QtGui/QResizeEvent>

#include "amultiimagewidget.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
AMultiImageWidget::AMultiImageWidget(QWidget *parent) : AImageWidget(parent) {
    _preview_img_wdg = new AImageWidget(this);
    _preview_img_wdg->setFixedSize(160,120);
    _preview_img_wdg->setVisible(false);

    QPalette palette = _preview_img_wdg->palette();
    palette.setColor(QPalette::Base, Qt::gray);
    palette.setColor(QPalette::Background, Qt::gray);
    _preview_img_wdg->setPalette(palette);

    QMetaObject::invokeMethod(this, "updatePreviewPosition"
        , Qt::QueuedConnection);
}


// ========================================================================== //
// Get preview image.
// ========================================================================== //
QImage AMultiImageWidget::previewImage() const {
    return _preview_img_wdg->image();
}


// ========================================================================== //
// Set preview image.
// ========================================================================== //
void AMultiImageWidget::setPreviewImage(const QImage &img) {
    _preview_img_wdg->setImage(img);
}


// ========================================================================== //
// Show preview.
// ========================================================================== //
void AMultiImageWidget::showPreview() {_preview_img_wdg->show();}


// ========================================================================== //
// Hide preview.
// ========================================================================== //
void AMultiImageWidget::hidePreview() {_preview_img_wdg->hide();}


// ========================================================================== //
// Resize event.
// ========================================================================== //
void AMultiImageWidget::resizeEvent(QResizeEvent *event) {
    AImageWidget::resizeEvent(event); updatePreviewPosition();
}


// ========================================================================== //
// Update preview position.
// ========================================================================== //
void AMultiImageWidget::updatePreviewPosition() {
    const int x = this->width() - _preview_img_wdg->width() - 4;
    const int y = 4;

    _preview_img_wdg->move(x, y);
}
