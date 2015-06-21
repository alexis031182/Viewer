#include <QtCore/QTimerEvent>

#include "aimagecapturedevice.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
AImageCaptureDevice::AImageCaptureDevice(QObject *parent)
    : ACaptureDevice(parent), _timer_id(0) {}


// ========================================================================== //
// Constructor.
// ========================================================================== //
AImageCaptureDevice::AImageCaptureDevice(const QString &fname, QObject *parent)
    : ACaptureDevice(parent), _timer_id(0), _fname(fname) {}


// ========================================================================== //
// Get is running.
// ========================================================================== //
bool AImageCaptureDevice::isRunning() const {
    return (_timer_id == 0) ? false : true;
}


// ========================================================================== //
// Get file name.
// ========================================================================== //
QString AImageCaptureDevice::fileName() const {return _fname;}


// ========================================================================== //
// Set file name.
// ========================================================================== //
void AImageCaptureDevice::setFileName(const QString &fname) {_fname = fname;}


// ========================================================================== //
// Start.
// ========================================================================== //
void AImageCaptureDevice::start() {
    if(isRunning()) return;

    emit starting();

    QImage img = QImage(_fname);
    if(img.isNull()) {
        emit failed(); return;
    }

    const double frm_rate = 4.;
    emit captureFpsChanged(frm_rate);

    int timer_id = startTimer(1000 / int(frm_rate));
    if(timer_id == 0) {
        emit failed(); return;
    }

    _timer_id = timer_id;

    _img = img.convertToFormat(QImage::Format_RGB888);

    emit started();
}


// ========================================================================== //
// Stop.
// ========================================================================== //
void AImageCaptureDevice::stop() {
    if(!isRunning()) return;

    emit stopping();

    killTimer(_timer_id); _timer_id = 0;

    _img = QImage();

    emit stopped();
}


// ========================================================================== //
// Timer event.
// ========================================================================== //
void AImageCaptureDevice::timerEvent(QTimerEvent *event) {
    if(event->timerId() == _timer_id) {
        QMetaObject::invokeMethod(this, "completed"
            , Qt::QueuedConnection, Q_ARG(QImage,_img));
    }
}
