#include <QtCore/QFileInfo>

#include <QtGui/QImageReader>

#include "adevicecontroller.h"
#include "avideocapturedevice.h"
#include "aimagecapturedevice.h"
#include "afilterdevice.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
ADeviceController::ADeviceController(QObject *parent)
    : QObject(parent), _cap_dev(NULL), _flt_dev(NULL) {}


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
    const bool is_capturing = isCapturing();
    if(is_capturing) stop();

    if(_cap_dev) {_cap_dev->deleteLater(); _cap_dev = NULL;}

    if(identifier.hasValue(ADeviceIdentifier::TYPE_URL)) {
        const QString url
            = identifier.value(ADeviceIdentifier::TYPE_URL).toString();

        const QString suffix = QFileInfo(url).suffix();

        bool has_img = false;

        QListIterator<QByteArray> itr(QImageReader::supportedImageFormats());
        while(itr.hasNext()) {
            if(itr.next() == suffix) {has_img = true; break;}
        }

        if(has_img) {
            _cap_dev = new AImageCaptureDevice(url, this);

        } else {
            AVideoCaptureDevice *cap_dev = new AVideoCaptureDevice(this);
            cap_dev->setDeviceName(url);

            _cap_dev = cap_dev;
        }

    } else {
        AVideoCaptureDevice *cap_dev = new AVideoCaptureDevice(this);
        if(identifier.hasValue(ADeviceIdentifier::TYPE_DEV)) {
            cap_dev->setDeviceName(identifier
                .value(ADeviceIdentifier::TYPE_DEV).toString());
        }

        if(identifier.hasValue(ADeviceIdentifier::TYPE_GRP)) {
            cap_dev->setGroupName(identifier
                .value(ADeviceIdentifier::TYPE_GRP).toString());
        }

        _cap_dev = cap_dev;
    }

    _identifier = identifier;

    connect(_cap_dev, &ACaptureDevice::completed
        , this, &ADeviceController::captured);
    connect(_cap_dev, &ACaptureDevice::failed
            , this, &ADeviceController::failed);
    connect(_cap_dev, &ACaptureDevice::captureFpsChanged
        , this, &ADeviceController::captureFpsChanged);

    if(_flt_dev) {
        connect(_cap_dev, &ACaptureDevice::started
            , _flt_dev, &AFilterDevice::start);
        connect(_cap_dev, &ACaptureDevice::stopped
            , _flt_dev, &AFilterDevice::stop);
        connect(_cap_dev, &ACaptureDevice::completed
            , _flt_dev, &AFilterDevice::run);
    }

    if(is_capturing) start();
}


// ========================================================================== //
// Get filter.
// ========================================================================== //
QString ADeviceController::filter() const {
    return (_flt_dev) ? _flt_dev->fileName() : QString();
}


// ========================================================================== //
// Set filter.
// ========================================================================== //
void ADeviceController::setFilter(const QString &fname) {
    unsetFilter();

    _flt_dev = new AFilterDevice(fname, this);
    connect(_flt_dev, &ACaptureDevice::completed
        , this, &ADeviceController::filtered);

    if(_cap_dev) {
        connect(_cap_dev, &ACaptureDevice::started
            , _flt_dev, &AFilterDevice::start);
        connect(_cap_dev, &ACaptureDevice::stopped
            , _flt_dev, &AFilterDevice::stop);
        connect(_cap_dev, &ACaptureDevice::completed
            , _flt_dev, &AFilterDevice::run);

        if(_cap_dev->isRunning())
            _flt_dev->start();
    }
}


// ========================================================================== //
// Unset filter.
// ========================================================================== //
void ADeviceController::unsetFilter() {
    if(_flt_dev) {
        if(_cap_dev) {
            disconnect(_cap_dev, &ACaptureDevice::started
                , _flt_dev, &AFilterDevice::start);
            disconnect(_cap_dev, &ACaptureDevice::stopped
                , _flt_dev, &AFilterDevice::stop);
            disconnect(_cap_dev, &ACaptureDevice::completed
                , _flt_dev, &AFilterDevice::run);
        }

        disconnect(_flt_dev, &ACaptureDevice::completed
            , this, &ADeviceController::filtered);

        if(_flt_dev->isRunning())
            _flt_dev->stop();

        _flt_dev->deleteLater(); _flt_dev = NULL;
    }
}


// ========================================================================== //
// Get filter properties.
// ========================================================================== //
QWidget *ADeviceController::filterProperties() const {
    return (_flt_dev) ? _flt_dev->properties() : NULL;
}


// ========================================================================== //
// Get is capturing.
// ========================================================================== //
bool ADeviceController::isCapturing() const {
    return (_cap_dev) ? _cap_dev->isRunning() : false;
}


// ========================================================================== //
// Start.
// ========================================================================== //
void ADeviceController::start() {
    if(_cap_dev && !_cap_dev->isRunning())
        _cap_dev->start();
}


// ========================================================================== //
// Stop.
// ========================================================================== //
void ADeviceController::stop() {
    if(_cap_dev && _cap_dev->isRunning())
        _cap_dev->stop();
}
