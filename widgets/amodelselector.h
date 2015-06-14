#ifndef AMODELSELECTOR_H
#define AMODELSELECTOR_H

#include <QtWidgets/QWidget>

class QAbstractItemModel;
class QTreeView;
class QLabel;

class AModelSelector : public QWidget {
    Q_OBJECT

    public:
        //! Constructor.
        explicit AModelSelector(QWidget *parent = NULL);

        //! Destructor.
        virtual ~AModelSelector() {}

        //! Get title.
        QString title() const;

        //! Set title.
        void setTitle(const QString &title);

        //! Get model.
        QAbstractItemModel *model() const;

        //! Set model.
        void setModel(QAbstractItemModel *model);

        //! Get current model index.
        QModelIndex currentIndex() const;

    private:
        QLabel *_label;

        QTreeView *_tview;

};

#endif
