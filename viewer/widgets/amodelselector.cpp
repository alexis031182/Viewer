#include <QtCore/QAbstractItemModel>

#include <QtWidgets/QTreeView>
#include <QtWidgets/QLayout>
#include <QtWidgets/QLabel>

#include "amodelselector.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
AModelSelector::AModelSelector(QWidget *parent) : QWidget(parent) {
    _label = new QLabel(this);
    _tview = new QTreeView(this);
    _tview->setEditTriggers(QAbstractItemView::NoEditTriggers);

    setLayout(new QVBoxLayout());
    layout()->addWidget(_label);
    layout()->addWidget(_tview);
}


// ========================================================================== //
// Get title.
// ========================================================================== //
QString AModelSelector::title() const {return _label->text();}


// ========================================================================== //
// Set title.
// ========================================================================== //
void AModelSelector::setTitle(const QString &title) {_label->setText(title);}


// ========================================================================== //
// Get model.
// ========================================================================== //
QAbstractItemModel *AModelSelector::model() const {return _tview->model();}


// ========================================================================== //
// Set model.
// ========================================================================== //
void AModelSelector::setModel(QAbstractItemModel *model) {
    _tview->setModel(model);
}


// ========================================================================== //
// Get current model index.
// ========================================================================== //
QModelIndex AModelSelector::currentIndex() const {
    return _tview->currentIndex();
}
