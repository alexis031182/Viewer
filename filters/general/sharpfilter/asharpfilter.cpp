#include <opencv2/imgproc.hpp>

#include "asharpfilter.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
ASharpFilter::ASharpFilter() : QObject() {}


// ========================================================================== //
// Get instance copy.
// ========================================================================== //
QSharedPointer<AFilterInterface> ASharpFilter::copy() const {
    return QSharedPointer<AFilterInterface>(new ASharpFilter());
}


// ========================================================================== //
// Get display group.
// ========================================================================== //
QString ASharpFilter::displayGroup() const {return ASharpFilter::tr("General");}


// ========================================================================== //
// Get display name.
// ========================================================================== //
QString ASharpFilter::displayName() const {return ASharpFilter::tr("Sharpen");}


// ========================================================================== //
// Get properties widget.
// ========================================================================== //
QWidget *ASharpFilter::properties() {return NULL;}


// ========================================================================== //
// Run.
// ========================================================================== //
void ASharpFilter::run(cv::Mat &matrix) {
    if(matrix.empty()) return;

    cv::Mat blr_mat;
    cv::GaussianBlur(matrix, blr_mat, cv::Size(0,0), 3);
    cv::addWeighted(matrix, 1.5, blr_mat, -0.5, 0, matrix);
}
