#include <opencv2/imgproc.hpp>

#include <QtWidgets/QComboBox>
#include <QtWidgets/QLayout>
#include <QtWidgets/QLabel>

#include "agrayfilter.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
AGrayFilter::AGrayFilter() : QObject(), _mode(MODE_GRAY) {}


// ========================================================================== //
// Get display group.
// ========================================================================== //
QString AGrayFilter::displayGroup() const {return AGrayFilter::tr("General");}


// ========================================================================== //
// Get display name.
// ========================================================================== //
QString AGrayFilter::displayName() const {return AGrayFilter::tr("Decolor");}


// ========================================================================== //
// Get properties widget.
// ========================================================================== //
QWidget *AGrayFilter::properties() {
    QWidget *main_wdg = new QWidget();
    main_wdg->setLayout(new QVBoxLayout());

    QLabel *mode_label = new QLabel(main_wdg);
    mode_label->setText(AGrayFilter::tr("Mode:"));

    QComboBox *mode_cbox = new QComboBox(main_wdg);
    mode_cbox->addItem(AGrayFilter::tr("Gray"), MODE_GRAY);
    mode_cbox->addItem(AGrayFilter::tr("R channel"), MODE_RGB_R);
    mode_cbox->addItem(AGrayFilter::tr("G channel"), MODE_RGB_G);
    mode_cbox->addItem(AGrayFilter::tr("B channel"), MODE_RGB_B);
    mode_cbox->addItem(AGrayFilter::tr("H channel"), MODE_HSV_H);
    mode_cbox->addItem(AGrayFilter::tr("S channel"), MODE_HSV_S);
    mode_cbox->addItem(AGrayFilter::tr("V channel"), MODE_HSV_V);

    _mutex.lock();
    mode_cbox->setCurrentIndex(mode_cbox->findData(_mode));
    _mutex.unlock();

    connect(mode_cbox
        , static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged)
        , [this](int idx) {

        QMutexLocker locker(&_mutex);
        _mode = (Mode)idx;
    });

    main_wdg->layout()->addWidget(mode_label);
    main_wdg->layout()->addWidget(mode_cbox);

    return main_wdg;
}


// ========================================================================== //
// Run.
// ========================================================================== //
void AGrayFilter::run(cv::Mat &matrix) {
    if(!matrix.empty() && matrix.channels() == 3) {
        _mutex.lock();
        Mode mode = _mode;
        _mutex.unlock();

        if(mode == MODE_GRAY) {
            cv::cvtColor(matrix, matrix, cv::COLOR_RGB2GRAY);
            cv::cvtColor(matrix, matrix, cv::COLOR_GRAY2RGB);

        } else {
            if(mode == MODE_HSV_H || mode == MODE_HSV_S
                || mode == MODE_HSV_V) {

                cv::cvtColor(matrix, matrix, cv::COLOR_RGB2HSV);
            }

            std::vector<cv::Mat> channels;
            cv::split(matrix, channels);

            if(mode == MODE_RGB_R || mode == MODE_HSV_H)
                matrix = channels.at(0);
            else if(mode == MODE_RGB_G || mode == MODE_HSV_S)
                matrix = channels.at(1);
            else if(mode == MODE_RGB_B || mode == MODE_HSV_V)
                matrix = channels.at(2);

            cv::cvtColor(matrix, matrix, cv::COLOR_GRAY2RGB);
        }
    }
}
