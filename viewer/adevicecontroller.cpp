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

    connect(_capture, &ACaptureThread::captured
        , this, &ADeviceController::captured);
    connect(_capture, &ACaptureThread::filtered
        , this, &ADeviceController::filtered);

    connect(_capture, &ACaptureThread::captureFpsChanged
        , this, &ADeviceController::captureFpsChanged);
    connect(_capture, &ACaptureThread::previewFpsChanged
        , this, &ADeviceController::previewFpsChanged);

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
