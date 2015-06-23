#include <QtCore/QPluginLoader>

#include <QtConcurrent/QtConcurrent>

#include <QtGui/QImage>

#include "afilterdevice.h"
#include "afilterinterface.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
AFilterDevice::AFilterDevice(QObject *parent)
    : ADevice(parent), _running(false) {}


// ========================================================================== //
// Constructor.
// ========================================================================== //
AFilterDevice::AFilterDevice(const QString &fname, QObject *parent)
    : ADevice(parent), _running(false) {setFileName(fname);}


// ========================================================================== //
// Get is running.
// ========================================================================== //
bool AFilterDevice::isRunning() const {return _running;}


// ========================================================================== //
// Get file name.
// ========================================================================== //
QString AFilterDevice::fileName() const {return _fname;}


// ========================================================================== //
// Set file name.
// ========================================================================== //
void AFilterDevice::setFileName(const QString &fname) {
    _fname = fname;

    QPluginLoader plugin;
    plugin.setFileName(fname);

    AFilterInterface *filter
        = qobject_cast<AFilterInterface*>(plugin.instance());

    const bool running = isRunning();
    if(running) stop();

    if(filter) _filter = filter->copy();
    else _filter.clear();

    if(running) start();
}


// ========================================================================== //
// Get properties.
// ========================================================================== //
QWidget *AFilterDevice::properties() const {
    if(_filter.isNull()) return NULL;
    return _filter->properties();
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
    if(!img.isNull() && _semaphore.tryAcquire())
        QtConcurrent::run(this, &AFilterDevice::threadRun, img);
}


// ========================================================================== //
// Thread run.
// ========================================================================== //
void AFilterDevice::threadRun(const QImage &src_img) {
    if(_filter.isNull()) {_semaphore.release(); return;}

    QImage img;
    if(src_img.format() == QImage::Format_RGB888) img = src_img;
    else img = src_img.convertToFormat(QImage::Format_RGB888);

    cv::Mat mat
        = cv::Mat(img.height(), img.width(), CV_8UC3, img.bits()
            , img.bytesPerLine()).clone();

    _filter->run(mat);

    img = QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);

    QMetaObject::invokeMethod(this, "completed"
        , Qt::QueuedConnection, Q_ARG(QImage,img.copy()));

    _semaphore.release();
}
