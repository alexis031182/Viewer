#ifndef AIMAGEWIDGET_H
#define AIMAGEWIDGET_H

#include <QtGui/QImage>

#include <QtWidgets/QWidget>

class AImageWidget : public QWidget {
    Q_OBJECT

    public:
        //! Constructor.
        explicit AImageWidget(QWidget *parent = NULL);

        //! Destructor.
        virtual ~AImageWidget() {}

        //! Get size hint.
        virtual QSize sizeHint() const;

        //! Get image.
        virtual QImage image() const;

    public slots:
        //! Set image.
        virtual void setImage(const QImage &img);

    protected:
        //! Paint event.
        virtual void paintEvent(QPaintEvent *event);

        //! Resize event.
        virtual void resizeEvent(QResizeEvent *event);

    private:
        QImage _img;

        QRect _dst_rc;

};

#endif
