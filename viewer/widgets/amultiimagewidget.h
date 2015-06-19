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

        //! Get preview image.
        QImage previewImage() const;

    public slots:
        //! Set preview image.
        void setPreviewImage(const QImage &img);

        //! Show preview.
        void showPreview();

        //! Hide preview.
        void hidePreview();

    protected:
        //! Resize event.
        virtual void resizeEvent(QResizeEvent *event);

    private:
        AImageWidget *_preview_img_wdg;

    private slots:
        //! Update preview position.
        void updatePreviewPosition();

};

#endif
