#ifndef AURLSELECTOR_H
#define AURLSELECTOR_H

#include <QtWidgets/QWidget>

class QLineEdit;
class QLabel;

class AUrlSelector : public QWidget {
    Q_OBJECT

    public:
        //! Constructor.
        explicit AUrlSelector(QWidget *parent = NULL);

        //! Destructor.
        virtual ~AUrlSelector() {}

        //! Get title.
        QString title() const;

        //! Set title.
        void setTitle(const QString &title);

        //! Get url.
        QString url() const;

        //! Set url.
        void setUrl(const QString &url);

    private:
        QLabel *_label;

        QLineEdit *_ledit;

};

#endif
