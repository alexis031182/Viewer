#include <QtGui/QResizeEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>

#include <QtWidgets/QStyleOptionFrame>

#include "aimagewidget.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
AImageWidget::AImageWidget(QWidget *parent) : QWidget(parent), _framed(false) {
    setAutoFillBackground(false);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QPalette palette = this->palette();
    palette.setColor(QPalette::Base, Qt::black);
    palette.setColor(QPalette::Background, Qt::black);
    setPalette(palette);
}


// ========================================================================== //
// Get size hint.
// ========================================================================== //
QSize AImageWidget::sizeHint() const {return QSize(320,240);}


// ========================================================================== //
// Has framed.
// ========================================================================== //
bool AImageWidget::hasFramed() const {return _framed;}


// ========================================================================== //
// Set framed.
// ========================================================================== //
void AImageWidget::setFramed(bool value) {
    const bool need_update = (_framed != value);

    _framed = value;

    if(need_update) update();
}


// ========================================================================== //
// Get image.
// ========================================================================== //
QImage AImageWidget::image() const {return _img;}


// ========================================================================== //
// Set image.
// ========================================================================== //
void AImageWidget::setImage(const QImage &img) {
    if(img.isNull()) {_img = QImage(); _dst_rc = QRect();}
    else {
        if(img.width() != _img.width() || img.height() != _img.height())
            _dst_rc = QRect();

        if(img.format() == QImage::Format_ARGB32_Premultiplied) _img = img;
        else _img = img.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    }

    update();
}


// ========================================================================== //
// Paint event.
// ========================================================================== //
void AImageWidget::paintEvent(QPaintEvent *event) {
    if(_img.isNull()) {
        QPainter painter(this);
        painter.fillRect(event->rect(), palette().background());
        event->accept();

        return;
    }

    if(_dst_rc.isNull()) {
        QSize img_sz = _img.size();
        QSize rc_sz  = rect().size();

        if(img_sz.width() > rc_sz.width() || img_sz.height() > rc_sz.height())
            img_sz.scale(rc_sz, Qt::KeepAspectRatio);

        QRect dst_rc(QPoint(0,0), img_sz);
        dst_rc.moveCenter(rect().center());

        _dst_rc = dst_rc;

        if(_framed) _dst_rc.adjust(2,2,-2,-2);
    }

    QPainter painter(this);

    if(!_dst_rc.contains(event->rect())) {
        QRegion region = event->region();
        region = region.subtracted(_dst_rc);

        const QBrush &brush = palette().background();
        foreach(const QRect &rc, region.rects())
            painter.fillRect(rc, brush);
    }

    painter.drawImage(_dst_rc, _img, _img.rect());

    if(_framed) {
        QStyleOptionFrame option;
        option.rect         = rect();
        option.midLineWidth = 1;
        option.lineWidth    = 1;
        option.frameShape   = QFrame::Panel;
        option.state        = QStyle::State_Raised;

        style()->drawControl(QStyle::CE_ShapedFrame, &option, &painter, this);
    }

    event->accept();
}


// ========================================================================== //
// Resize event.
// ========================================================================== //
void AImageWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    if(!_dst_rc.isNull()) _dst_rc = QRect();
}
