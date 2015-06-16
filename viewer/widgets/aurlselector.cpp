#include <QtCore/QCoreApplication>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLayout>
#include <QtWidgets/QLabel>

#include "aurlselector.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
AUrlSelector::AUrlSelector(QWidget *parent) : QWidget(parent) {
    _label = new QLabel(this);
    _ledit = new QLineEdit(this);

    QToolButton *tbut = new QToolButton(this);
    tbut->setText(QStringLiteral("..."));
    connect(tbut, &QToolButton::clicked, [this]() {
        const QString url
            = QFileDialog::getOpenFileName(this
                , AUrlSelector::tr("Select resource:")
                , (_ledit->text().isEmpty())
                    ? QCoreApplication::applicationDirPath()
                    : _ledit->text());

        if(!url.isEmpty()) _ledit->setText(url);
    });

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(_label, 0, 0, 1, 2);
    layout->addWidget(_ledit, 1, 0, 1, 1);
    layout->addWidget(tbut, 1, 1, 1, 1);

    setLayout(layout);
}


// ========================================================================== //
// Get title.
// ========================================================================== //
QString AUrlSelector::title() const {return _label->text();}


// ========================================================================== //
// Set title.
// ========================================================================== //
void AUrlSelector::setTitle(const QString &title) {_label->setText(title);}


// ========================================================================== //
// Get url.
// ========================================================================== //
QString AUrlSelector::url() const {return _ledit->text();}


// ========================================================================== //
// Set url.
// ========================================================================== //
void AUrlSelector::setUrl(const QString &url) {_ledit->setText(url);}
