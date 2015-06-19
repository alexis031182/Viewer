#ifndef AMULTIIMAGEWIDGET_H
#define AMULTIIMAGEWIDGET_H

#include "aimagewidget.h"

class AMultiImageWidget : public AImageWidget {
    Q_OBJECT

    public:
        //! Constructor.
        explicit AMultiImageWidget(QWidget *parent = NULL);

        //! Destructor.
        virtual ~AMultiImageWidget() {}

        //! Get image.
        virtual QImage image() const;

        //! Get preview image.
        QImage previewImage() const;

    public slots:
        //! Set image.
        virtual void setImage(const QImage &img);

        //! Set preview image.
        void setPreviewImage(const QImage &img);

        //! Show preview.
        void showPreview();

        //! Hide preview.
        void hidePreview();

        //! Switch preview.
        void switchPreview();

    protected:
        //! Resize event.
        virtual void resizeEvent(QResizeEvent *event);

        //! Event filter.
        virtual bool eventFilter(QObject *obj, QEvent *event);

    private:
        bool _preview_switched;

        AImageWidget *_preview_img_wdg;

    private slots:
        //! Update preview position.
        void updatePreviewPosition();

};

#endif
