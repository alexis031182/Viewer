#include <opencv2/imgproc.hpp>

#include <QtWidgets/QComboBox>
#include <QtWidgets/QLayout>
#include <QtWidgets/QLabel>

#include "ablurfilter.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
ABlurFilter::ABlurFilter() : QObject(), _type(TYPE_NORMALIZED), _krnl_size(3) {}


// ========================================================================== //
// Get instance copy.
// ========================================================================== //
QSharedPointer<AFilterInterface> ABlurFilter::copy() const {
    return QSharedPointer<AFilterInterface>(new ABlurFilter());
}


// ========================================================================== //
// Get display group.
// ========================================================================== //
QString ABlurFilter::displayGroup() const {return ABlurFilter::tr("General");}


// ========================================================================== //
// Get display name.
// ========================================================================== //
QString ABlurFilter::displayName() const {return ABlurFilter::tr("Blur");}


// ========================================================================== //
// Get properties widget.
// ========================================================================== //
QWidget *ABlurFilter::properties() {
    QWidget *main_wdg = new QWidget();

    QLabel *type_label = new QLabel(main_wdg);
    type_label->setText(ABlurFilter::tr("Filter type:"));

    QComboBox *type_cbox = new QComboBox(main_wdg);
    type_cbox->addItem(ABlurFilter::tr("Normalized box"), TYPE_NORMALIZED);
    type_cbox->addItem(ABlurFilter::tr("Gaussian"), TYPE_GAUSSIAN);
    type_cbox->addItem(ABlurFilter::tr("Median"), TYPE_MEDIAN);

    _mutex.lock();
    type_cbox->setCurrentIndex(type_cbox->findData(_type));
    _mutex.unlock();

    connect(type_cbox
        , static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged)
        , [this](int idx) {

        QMutexLocker locker(&_mutex);
        _type = (Type)idx;
    });

    QLabel *krnl_label = new QLabel(main_wdg);
    krnl_label->setText(ABlurFilter::tr("Kernel size:"));

    QComboBox *krnl_cbox = new QComboBox(main_wdg);
    for(int i = 3; i < 53; i += 2)
        krnl_cbox->addItem(QString::number(i), i);

    _mutex.lock();
    krnl_cbox->setCurrentIndex(krnl_cbox->findData(_krnl_size));
    _mutex.unlock();

    connect(krnl_cbox
        , static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged)
        , [this,krnl_cbox](int idx) {

        QMutexLocker locker(&_mutex);
        _krnl_size = krnl_cbox->itemData(idx).toInt();
    });

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(type_label, 0, 0, 1, 1);
    layout->addWidget(type_cbox, 0, 1, 1, 1);
    layout->addWidget(krnl_label, 1, 0, 1, 1);
    layout->addWidget(krnl_cbox, 1, 1, 1, 1);

    main_wdg->setLayout(layout);

    return main_wdg;
}


// ========================================================================== //
// Run.
// ========================================================================== //
void ABlurFilter::run(cv::Mat &matrix) {
    if(matrix.empty()) return;

    _mutex.lock();
    Type type     = _type;
    int krnl_size = _krnl_size;
    _mutex.unlock();

    switch(type) {
        case TYPE_NORMALIZED:
            cv::blur(matrix, matrix, cv::Size(krnl_size,krnl_size));
        break;

        case TYPE_GAUSSIAN:
            cv::GaussianBlur(matrix, matrix, cv::Size(krnl_size,krnl_size), 0);
        break;

        case TYPE_MEDIAN:
            cv::medianBlur(matrix, matrix, krnl_size);
        break;
    }
}
