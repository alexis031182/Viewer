#ifndef ASOURCESELECTOR_H
#define ASOURCESELECTOR_H

#include <QtWidgets/QWidget>

class QTabWidget;
class QLineEdit;
class QComboBox;

class ASourceSelector : public QWidget {
    Q_OBJECT
    Q_ENUMS(Type)

    public:
        enum Type {TYPE_URL, TYPE_DEV};

        class FullName {
            public:
                //! Constructor.
                FullName(const QString &short_name)
                    : _short_name(short_name) {}

                //! Constructor.
                FullName(const QString &short_name, const QString &long_name)
                    : _short_name(short_name), _long_name(long_name) {}

                //! Get short name.
                inline const QString &shortName() const {return _short_name;}

                //! Get long name.
                inline const QString &longName() const {return _long_name;}

            private:
                const QString _short_name, _long_name;

        };

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

        //! Get groups.
        QList<ASourceSelector::FullName> groups() const;

        //! Set groups.
        void setGroups(const QList<ASourceSelector::FullName> &groups);

        //! Get devices.
        QList<ASourceSelector::FullName> devices() const;

        //! Set devices.
        void setDevices(const QList<ASourceSelector::FullName> &devices);

    private:
        QTabWidget *_type_tab_wdg;

        QLineEdit *_url_ledit;

        QComboBox *_dev_grp_cbox, *_dev_dev_cbox;

        //! Get names.
        QList<ASourceSelector::FullName> names(QComboBox *cbox) const;

        //! Set names.
        void setNames(const QList<ASourceSelector::FullName> &names
            , QComboBox *cbox);

};

#endif
