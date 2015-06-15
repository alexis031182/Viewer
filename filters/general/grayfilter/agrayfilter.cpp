#include <opencv2/imgproc.hpp>

#include "agrayfilter.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
AGrayFilter::AGrayFilter() : QObject() {}


// ========================================================================== //
// Get display group.
// ========================================================================== //
QString AGrayFilter::displayGroup() const {return AGrayFilter::tr("General");}


// ========================================================================== //
// Get display name.
// ========================================================================== //
QString AGrayFilter::displayName() const {return AGrayFilter::tr("Decolor");}


// ========================================================================== //
// Use.
// ========================================================================== //
void AGrayFilter::use(QWidget *parent) {Q_UNUSED(parent);}


// ========================================================================== //
// Run.
// ========================================================================== //
void AGrayFilter::run(cv::Mat &matrix) {
    if(!matrix.empty() && matrix.channels() == 3) {
        cv::cvtColor(matrix, matrix, cv::COLOR_RGB2GRAY);
        cv::cvtColor(matrix, matrix, cv::COLOR_GRAY2RGB);
    }
}
