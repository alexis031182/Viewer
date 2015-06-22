#include <QtCore/QPluginLoader>

#include <QtConcurrent/QtConcurrent>

#include <QtGui/QImage>

#include "afilterdevice.h"
#include "afilterinterface.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
AFilterDevice::AFilterDevice(QObject *parent)
    : ADevice(parent), _running(false)
    , _plugin(new QPluginLoader(this)) {}


// ========================================================================== //
// Constructor.
// ========================================================================== //
AFilterDevice::AFilterDevice(const QString &fname, QObject *parent)
    : ADevice(parent), _running(false)
    , _plugin(new QPluginLoader(fname,this)) {_plugin->load();}


// ========================================================================== //
// Get is running.
// ========================================================================== //
bool AFilterDevice::isRunning() const {return _running;}


// ========================================================================== //
// Get file name.
// ========================================================================== //
QString AFilterDevice::fileName() const {return _plugin->fileName();}


// ========================================================================== //
// Set file name.
// ========================================================================== //
void AFilterDevice::setFileName(const QString &fname) {
    if(_plugin->isLoaded())
        _plugin->unload();

    _plugin->setFileName(fname);
    _plugin->load();
}


// ========================================================================== //
// Get properties.
// ========================================================================== //
QWidget *AFilterDevice::properties() const {
    if(_plugin->isLoaded()) {
        AFilterInterface *filter
            = qobject_cast<AFilterInterface*>(_plugin->instance());

        if(filter) return filter->properties();
    }

    return NULL;
}


// ========================================================================== //
// Start.
// ========================================================================== //
void AFilterDevice::start() {
    if(isRunning()) return;

    emit starting();

    _running = true;

    _semaphore.release();

    emit started();
}


// ========================================================================== //
// Stop.
// ========================================================================== //
void AFilterDevice::stop() {
    if(!isRunning()) return;

    emit stopping();

    _semaphore.acquire();

    _running = false;

    emit stopped();
}


// ========================================================================== //
// Run.
// ========================================================================== //
void AFilterDevice::run(const QImage &img) {
    if(_semaphore.tryAcquire(1))
        QtConcurrent::run(this, &AFilterDevice::threadRun, img);
}


// ========================================================================== //
// Thread run.
// ========================================================================== //
void AFilterDevice::threadRun(const QImage &src_img) {
    AFilterInterface *filter
        = qobject_cast<AFilterInterface*>(_plugin->instance());

    if(!filter) {_semaphore.release(); return;}

    QImage img;
    if(src_img.format() == QImage::Format_RGB888) img = src_img;
    else img = src_img.convertToFormat(QImage::Format_RGB888);

    cv::Mat mat
        = cv::Mat(img.height(), img.width(), CV_8UC3, img.bits()
            , img.bytesPerLine()).clone();

    filter->run(mat);

    img = QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);

    QMetaObject::invokeMethod(this, "completed"
        , Qt::QueuedConnection, Q_ARG(QImage,img.copy()));

    _semaphore.release();
}
