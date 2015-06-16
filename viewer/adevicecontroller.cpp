#include <QtCore/QPluginLoader>

#include "widgets/aimagewidget.h"

#include "adevicecontroller.h"
#include "afilterinterface.h"
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
// Get device identifier.
// ========================================================================== //
const ADeviceIdentifier &ADeviceController::identifier() const {
    return _identifier;
}


// ========================================================================== //
// Set device identifier.
// ========================================================================== //
void ADeviceController::setIdentifier(const ADeviceIdentifier &identifier) {
    const bool is_capturing = _capture->isRunning();
    if(is_capturing) stop();

    _identifier = identifier;

    if(_identifier.hasValue(ADeviceIdentifier::TYPE_URL)) {
        _capture->setDeviceName(_identifier
            .value(ADeviceIdentifier::TYPE_URL).toString());

    } else {
        if(_identifier.hasValue(ADeviceIdentifier::TYPE_DEV)) {
            _capture->setDeviceName(_identifier
                .value(ADeviceIdentifier::TYPE_DEV).toString());
        }

        if(_identifier.hasValue(ADeviceIdentifier::TYPE_GRP)) {
            _capture->setGroupName(_identifier
                .value(ADeviceIdentifier::TYPE_GRP).toString());
        }
    }

    if(is_capturing) start();
}


// ========================================================================== //
// Get filter.
// ========================================================================== //
QString ADeviceController::filter() const {
    return (_loader) ? _loader->fileName() : QString();
}


// ========================================================================== //
// Set filter.
// ========================================================================== //
void ADeviceController::setFilter(const QString &fname) {
    unsetFilter();

    _loader = new QPluginLoader(fname, this);

    QObject *filter_obj = _loader->instance();
    if(filter_obj) {
        _capture->setFilter(qobject_cast<AFilterInterface*>(filter_obj));

    } else unsetFilter();
}


// ========================================================================== //
// Unset filter.
// ========================================================================== //
void ADeviceController::unsetFilter() {
    if(_loader) {
        if(_loader->isLoaded()) {_capture->unsetFilter(); _loader->unload();}

        _loader->deleteLater(); _loader = NULL;
    }
}


// ========================================================================== //
// Get filter properties.
// ========================================================================== //
QWidget *ADeviceController::filterProperties() const {
    if(_loader) {
        AFilterInterface *filter
            = qobject_cast<AFilterInterface*>(_loader->instance());

        if(filter) return filter->properties();
    }

    return NULL;
}


// ========================================================================== //
// Get is capturing.
// ========================================================================== //
bool ADeviceController::isCapturing() const {return _capture->isRunning();}


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

        _img_wdg->setImage(QImage());
    }

    _img_wdg = wdg;

    if(_img_wdg) {
        connect(_capture, &ACaptureThread::captured
            , _img_wdg, &AImageWidget::setImage);
    }
}


// ========================================================================== //
// Unset image widget.
// ========================================================================== //
void ADeviceController::unsetImageWidget() {setImageWidget(NULL);}


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
