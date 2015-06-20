#include <opencv2/imgproc.hpp>

#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QLayout>
#include <QtWidgets/QLabel>

#include "abutterworthfilter.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
AButterworthFilter::AButterworthFilter() : QObject()
    , _type(TYPE_LOWPASS), _order(2), _radius(30) {}


// ========================================================================== //
// Get display group.
// ========================================================================== //
QString AButterworthFilter::displayGroup() const {
    return AButterworthFilter::tr("Frequency");
}


// ========================================================================== //
// Get display name.
// ========================================================================== //
QString AButterworthFilter::displayName() const {
    return AButterworthFilter::tr("Butterworth filter");
}


// ========================================================================== //
// Get properties widget.
// ========================================================================== //
QWidget *AButterworthFilter::properties() {
    QWidget *main_wdg = new QWidget();

    QLabel *type_label = new QLabel(main_wdg);
    type_label->setText(AButterworthFilter::tr("Filter type:"));

    QComboBox *type_cbox = new QComboBox(main_wdg);
    type_cbox->addItem(AButterworthFilter::tr("Low pass"), TYPE_LOWPASS);
    type_cbox->addItem(AButterworthFilter::tr("High pass"), TYPE_HIGHPASS);

    _mutex.lock();
    type_cbox->setCurrentIndex(type_cbox->findData(_type));
    _mutex.unlock();

    connect(type_cbox
        , static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged)
        , [this](int idx) {

        QMutexLocker locker(&_mutex);
        _type    = (Type)idx;
        _krn_mat = cv::Mat();
    });

    QLabel *order_label = new QLabel(main_wdg);
    order_label->setText(AButterworthFilter::tr("Order:"));

    QSpinBox *order_sbox = new QSpinBox(main_wdg);
    order_sbox->setRange(1,10);

    _mutex.lock();
    order_sbox->setValue(_order);
    _mutex.unlock();

    connect(order_sbox
        , static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged)
        , [this](int value) {

        QMutexLocker locker(&_mutex);
        _order   = value;
        _krn_mat = cv::Mat();
    });

    QLabel *radius_label = new QLabel(main_wdg);
    radius_label->setText(AButterworthFilter::tr("Radius:"));

    QSpinBox *radius_sbox = new QSpinBox(main_wdg);

    int max_radius = 100;

    _mutex.lock();

    if(!_krn_mat.empty())
        max_radius = std::min(_krn_mat.rows,_krn_mat.cols) / 2;

    radius_sbox->setRange(2,max_radius);
    radius_sbox->setValue(_radius);

    _mutex.unlock();

    connect(radius_sbox
        , static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged)
        , [this](int value) {

        QMutexLocker locker(&_mutex);
        _radius  = value;
        _krn_mat = cv::Mat();
    });

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(type_label, 0, 0, 1, 1);
    layout->addWidget(type_cbox, 0, 1, 1, 1);
    layout->addWidget(order_label, 1, 0, 1, 1);
    layout->addWidget(order_sbox, 1, 1, 1, 1);
    layout->addWidget(radius_label, 2, 0, 1, 1);
    layout->addWidget(radius_sbox, 2, 1, 1, 1);

    main_wdg->setLayout(layout);

    return main_wdg;
}


// ========================================================================== //
// Run.
// ========================================================================== //
void AButterworthFilter::run(cv::Mat &matrix) {
    if(matrix.empty()) return;

    cv::Mat gry_mat;
    switch(matrix.channels()) {
        case 1: gry_mat = matrix.clone(); break;
        case 3: cv::cvtColor(matrix, gry_mat, cv::COLOR_RGB2GRAY); break;
        default: return;
    }

    const int opt_rows = cv::getOptimalDFTSize(gry_mat.rows);
    const int opt_cols = cv::getOptimalDFTSize(gry_mat.cols);

    cv::Mat pad_mat;
    cv::copyMakeBorder(gry_mat, pad_mat
        , 0, opt_rows - gry_mat.rows
        , 0, opt_cols - gry_mat.cols
        , cv::BORDER_CONSTANT
        , cv::Scalar::all(0));

    std::vector<cv::Mat> planes;
    planes.push_back(cv::Mat_<float>(pad_mat));
    planes.push_back(cv::Mat::zeros(pad_mat.size(),CV_32F));

    cv::Mat cpx_mat;
    cv::merge(planes, cpx_mat);
    cv::dft(cpx_mat, cpx_mat);

    _mutex.lock();

    if(_krn_mat.empty() || _krn_mat.size() != cpx_mat.size())
        createKernel(cpx_mat.size(), _type, _order, _radius, _krn_mat);

    shiftDft(cpx_mat);
    cv::mulSpectrums(cpx_mat, _krn_mat, cpx_mat, 0);
    shiftDft(cpx_mat);

    _mutex.unlock();

    cv::idft(cpx_mat, cpx_mat);
    cv::split(cpx_mat, planes);
    cv::normalize(planes[0], matrix, 0, 255, cv::NORM_MINMAX);
    matrix.convertTo(matrix, CV_8U);
    cv::cvtColor(matrix, matrix, cv::COLOR_GRAY2RGB);
}


// ========================================================================== //
// Create kernel.
// ========================================================================== //
void AButterworthFilter::createKernel(const cv::Size &size, Type type
    , int order, int radius, cv::Mat &mat) const {

    cv::Point pnt = cv::Point(size.height / 2, size.width / 2);

    cv::Mat tmp_mat = cv::Mat(size, CV_32F);
    for(int row = 0; row < tmp_mat.rows; ++row) {
        for(int col = 0; col < tmp_mat.cols; ++col) {
            const double r
                = std::sqrt(std::pow(double(row-pnt.x),2.0)
                    + std::pow(double(col-pnt.y),2.0));

            switch(type) {
                case TYPE_LOWPASS:
                    tmp_mat.at<float>(row,col)
                        = float(1 / (1 + std::pow(double(r/radius)
                            , double(order*2))));

                break;

                case TYPE_HIGHPASS:
                    tmp_mat.at<float>(row,col)
                        = float(1 / (1 + std::pow(double(radius/r)
                            , double(order*2))));
                break;
            }
        }
    }

    std::vector<cv::Mat> channels;
    channels.push_back(tmp_mat);
    channels.push_back(tmp_mat);
    cv::merge(channels, mat);
}


// ========================================================================== //
// Shift dft.
// ========================================================================== //
void AButterworthFilter::shiftDft(cv::Mat &mat) const {
    mat = mat(cv::Rect(0,0,mat.cols&-2,mat.rows&-2));

    const int hy = mat.rows / 2;
    const int hx = mat.cols / 2;

    cv::Mat rc0_mat(mat, cv::Rect(0,0,hx,hy));
    cv::Mat rc1_mat(mat, cv::Rect(hx,0,hx,hy));
    cv::Mat rc2_mat(mat, cv::Rect(0,hy,hx,hy));
    cv::Mat rc3_mat(mat, cv::Rect(hx,hy,hx,hy));

    cv::Mat tmp_mat;
    rc0_mat.copyTo(tmp_mat);
    rc3_mat.copyTo(rc0_mat);
    tmp_mat.copyTo(rc3_mat);

    rc1_mat.copyTo(tmp_mat);
    rc2_mat.copyTo(rc1_mat);
    tmp_mat.copyTo(rc2_mat);
}
