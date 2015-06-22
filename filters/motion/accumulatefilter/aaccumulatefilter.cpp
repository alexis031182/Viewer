#include <opencv2/imgproc.hpp>

#include <QtWidgets/QLayout>
#include <QtWidgets/QLabel>

#include "aaccumulatefilter.h"
#include "asliderwidget.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
AAccumulateFilter::AAccumulateFilter() : QObject(), _alpha(1000) {}


// ========================================================================== //
// Get display group.
// ========================================================================== //
QString AAccumulateFilter::displayGroup() const {
    return AAccumulateFilter::tr("Motion");
}


// ========================================================================== //
// Get display name.
// ========================================================================== //
QString AAccumulateFilter::displayName() const {
    return AAccumulateFilter::tr("Accumulate");
}


// ========================================================================== //
// Get properties widget.
// ========================================================================== //
QWidget *AAccumulateFilter::properties() {
    QWidget *main_wdg = new QWidget();

    QLabel *label = new QLabel(main_wdg);
    label->setText(AAccumulateFilter::tr("Coefficient:"));

    ASliderWidget *swdg = new ASliderWidget(main_wdg);
    swdg->setRange(1, 1000);

    _mutex.lock();
    swdg->setValue(_alpha);
    _mutex.unlock();

    connect(swdg, &ASliderWidget::valueChanged, [this](int value) {
        QMutexLocker locker(&_mutex);
        _alpha = value;
    });

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(label, 0, 0, 1, 1);
    layout->addWidget(swdg, 1, 0, 1, 1);

    main_wdg->setLayout(layout);

    return main_wdg;
}


// ========================================================================== //
// Run.
// ========================================================================== //
void AAccumulateFilter::run(cv::Mat &matrix) {
    if(matrix.empty()) return;

    _mutex.lock();

    double alpha = double(_alpha);

    if(_acc_mat.empty() || _acc_mat.size() != matrix.size())
        _acc_mat = cv::Mat(matrix.size(), CV_64FC3);

    _mutex.unlock();

    cv::accumulateWeighted(matrix, _acc_mat, alpha * 0.001);
    cv::convertScaleAbs(_acc_mat, matrix);
}
