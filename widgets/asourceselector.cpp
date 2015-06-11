#include <QtWidgets/QTabWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>
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

    QLabel *dev_grp_label = new QLabel(this);
    dev_grp_label->setText(ASourceSelector::tr("Category:"));

    _dev_grp_cbox = new QComboBox(this);

    QLabel *dev_dev_label = new QLabel(this);
    dev_dev_label->setText(ASourceSelector::tr("Device:"));

    _dev_dev_cbox = new QComboBox(this);

    QWidget *dev_wdg = new QWidget(this);
    dev_wdg->setLayout(new QVBoxLayout());
    dev_wdg->layout()->addWidget(dev_grp_label);
    dev_wdg->layout()->addWidget(_dev_grp_cbox);
    dev_wdg->layout()->addWidget(dev_dev_label);
    dev_wdg->layout()->addWidget(_dev_dev_cbox);

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
// Get groups.
// ========================================================================== //
QList<ASourceSelector::FullName> ASourceSelector::groups() const {
    return names(_dev_grp_cbox);
}


// ========================================================================== //
// Set groups.
// ========================================================================== //
void ASourceSelector::setGroups(const QList<FullName> &groups) {
    setNames(groups, _dev_grp_cbox);
}


// ========================================================================== //
// Get devices.
// ========================================================================== //
QList<ASourceSelector::FullName> ASourceSelector::devices() const {
    return names(_dev_dev_cbox);
}


// ========================================================================== //
// Set devices.
// ========================================================================== //
void ASourceSelector::setDevices(const QList<FullName> &devices) {
    setNames(devices, _dev_dev_cbox);
}


// ========================================================================== //
// Get names.
// ========================================================================== //
QList<ASourceSelector::FullName> ASourceSelector::names(QComboBox *cbox) const {
    QList<ASourceSelector::FullName> names;
    for(int i = 0, n = cbox->count(); i < n; ++i)
        names.append(FullName(cbox->itemText(i),cbox->itemData(i).toString()));

    return names;
}


// ========================================================================== //
// Set names.
// ========================================================================== //
void ASourceSelector::setNames(const QList<ASourceSelector::FullName> &names
    , QComboBox *cbox) {

    cbox->clear();

    QListIterator<FullName> itr(names);
    while(itr.hasNext()) {
        const FullName &name = itr.next();
        cbox->addItem(name.shortName(),name.longName());
    }
}
