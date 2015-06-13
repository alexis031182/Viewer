#include "widgets/aimagewidget.h"

#include "adevicecontroller.h"
#include "acapturethread.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
ADeviceController::ADeviceController(QObject *parent)
    : QObject(parent), _capture(new ACaptureThread(this)) {

    connect(_capture, &ACaptureThread::failed
        , this, &ADeviceController::failed);
    connect(_capture, &ACaptureThread::finished
        , this, &ADeviceController::stop);
}


// ========================================================================== //
// Get image widget.
// ========================================================================== //
AImageWidget *ADeviceController::imageWidget() const {return _img_wdg;}


// ========================================================================== //
// Set image widget.
// ========================================================================== //
void ADeviceController::setImageWidget(AImageWidget *wdg) {
    if(_img_wdg) {
        disconnect(_capture, &ACaptureThread::captured
            , _img_wdg, &AImageWidget::setImage);
    }

    _img_wdg = wdg;

    if(_img_wdg) {
        connect(_capture, &ACaptureThread::captured
            , _img_wdg, &AImageWidget::setImage);
    }
}


// ========================================================================== //
// Start.
// ========================================================================== //
void ADeviceController::start() {
    if(_capture->isRunning()) return;

    emit starting();

    _capture->start();

    emit started();
}


// ========================================================================== //
// Stop.
// ========================================================================== //
void ADeviceController::stop() {
    if(!_capture->isRunning()) return;

    emit stopping();

    disconnect(_capture, &ACaptureThread::finished
        , this, &ADeviceController::stop);

    _capture->requestInterruption();
    _capture->wait();

    connect(_capture, &ACaptureThread::finished
        , this, &ADeviceController::stop);

    emit stopped();
}
