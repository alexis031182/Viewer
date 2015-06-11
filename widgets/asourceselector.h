#ifndef ASOURCESELECTOR_H
#define ASOURCESELECTOR_H

#include <QtWidgets/QWidget>

class QAbstractItemModel;
class QTabWidget;
class QLineEdit;
class QTreeView;

class ASourceSelector : public QWidget {
    Q_OBJECT
    Q_ENUMS(Type)

    public:
        enum Type {TYPE_URL, TYPE_DEV};

        //! Constructor.
        explicit ASourceSelector(QWidget *parent = NULL);

        //! Destructor.
        virtual ~ASourceSelector() {}

        //! Get current type.
        ASourceSelector::Type type() const;

        //! Set current type.
        void setType(ASourceSelector::Type type);

        //! Get url.
        QString url() const;

        //! Set url.
        void setUrl(const QString &url);

        //! Get video device model.
        QAbstractItemModel *videoDeviceModel() const;

        //! Set video device model.
        void setVideoDeviceModel(QAbstractItemModel *model);

    private:
        QTabWidget *_type_tab_wdg;

        QLineEdit *_url_ledit;

        QTreeView *_dev_tview;

};

#endif
