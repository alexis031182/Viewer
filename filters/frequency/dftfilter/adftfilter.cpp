#include <opencv2/imgproc.hpp>

#include "adftfilter.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
ADftFilter::ADftFilter() : QObject() {}


// ========================================================================== //
// Get instance copy.
// ========================================================================== //
QSharedPointer<AFilterInterface> ADftFilter::copy() const {
    return QSharedPointer<AFilterInterface>(new ADftFilter());
}


// ========================================================================== //
// Get display group.
// ========================================================================== //
QString ADftFilter::displayGroup() const {return ADftFilter::tr("Frequency");}


// ========================================================================== //
// Get display name.
// ========================================================================== //
QString ADftFilter::displayName() const {
    return ADftFilter::tr("Discrete Fourier Transform");
}


// ========================================================================== //
// Get properties widget.
// ========================================================================== //
QWidget *ADftFilter::properties() {return NULL;}


// ========================================================================== //
// Run.
// ========================================================================== //
void ADftFilter::run(cv::Mat &matrix) {
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
    cv::split(cpx_mat, planes);
    cv::magnitude(planes[0], planes[1], planes[0]);

    cv::Mat &mgn_mat = planes[0];
    mgn_mat += cv::Scalar::all(1);
    cv::log(mgn_mat, mgn_mat);

    mgn_mat = mgn_mat(cv::Rect(0,0,mgn_mat.cols&-2,mgn_mat.rows&-2));

    const int hy = mgn_mat.rows / 2;
    const int hx = mgn_mat.cols / 2;

    cv::Mat rc0_mat(mgn_mat, cv::Rect(0,0,hx,hy));
    cv::Mat rc1_mat(mgn_mat, cv::Rect(hx,0,hx,hy));
    cv::Mat rc2_mat(mgn_mat, cv::Rect(0,hy,hx,hy));
    cv::Mat rc3_mat(mgn_mat, cv::Rect(hx,hy,hx,hy));

    cv::Mat tmp_mat;
    rc0_mat.copyTo(tmp_mat);
    rc3_mat.copyTo(rc0_mat);
    tmp_mat.copyTo(rc3_mat);

    rc1_mat.copyTo(tmp_mat);
    rc2_mat.copyTo(rc1_mat);
    tmp_mat.copyTo(rc2_mat);

    cv::normalize(mgn_mat, mgn_mat, 0, 255, cv::NORM_MINMAX);
    mgn_mat.convertTo(matrix, CV_8U);
    cv::cvtColor(matrix, matrix, cv::COLOR_GRAY2RGB);
}
