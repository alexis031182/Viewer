#include <QtCore/QAbstractItemModel>

#include <QtWidgets/QTabWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QLayout>
#include <QtWidgets/QLabel>

#include "asourceselector.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
ASourceSelector::ASourceSelector(QWidget *parent) : QWidget(parent) {
    QLabel *url_label = new QLabel(this);
    url_label->setText(ASourceSelector::tr("Uniform Resource Locator:"));

    _url_ledit = new QLineEdit(this);

    QWidget *url_wdg = new QWidget(this);
    url_wdg->setLayout(new QVBoxLayout());
    url_wdg->layout()->addWidget(url_label);
    url_wdg->layout()->addWidget(_url_ledit);

    QLabel *dev_label = new QLabel(this);
    dev_label->setText(ASourceSelector::tr("Available video devices:"));

    _dev_tview = new QTreeView(this);

    QWidget *dev_wdg = new QWidget(this);
    dev_wdg->setLayout(new QVBoxLayout());
    dev_wdg->layout()->addWidget(dev_label);
    dev_wdg->layout()->addWidget(_dev_tview);

    _type_tab_wdg = new QTabWidget(this);
    _type_tab_wdg->addTab(url_wdg, ASourceSelector::tr("URL"));
    _type_tab_wdg->addTab(dev_wdg, ASourceSelector::tr("Device"));

    setLayout(new QVBoxLayout());
    layout()->addWidget(_type_tab_wdg);
}


// ========================================================================== //
// Get current type.
// ========================================================================== //
ASourceSelector::Type ASourceSelector::type() const {
    return (ASourceSelector::Type)_type_tab_wdg->currentIndex();
}


// ========================================================================== //
// Set current type.
// ========================================================================== //
void ASourceSelector::setType(ASourceSelector::Type type) {
    _type_tab_wdg->setCurrentIndex((int)type);
}


// ========================================================================== //
// Get url.
// ========================================================================== //
QString ASourceSelector::url() const {return _url_ledit->text();}


// ========================================================================== //
// Set url.
// ========================================================================== //
void ASourceSelector::setUrl(const QString &url) {_url_ledit->setText(url);}


// ========================================================================== //
// Get video device model.
// ========================================================================== //
QAbstractItemModel *ASourceSelector::videoDeviceModel() const {
    return _dev_tview->model();
}


// ========================================================================== //
// Set video device model.
// ========================================================================== //
void ASourceSelector::setVideoDeviceModel(QAbstractItemModel *model) {
    _dev_tview->setModel(model);
}
