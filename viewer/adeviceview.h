#ifndef ADEVICEVIEW_H
#define ADEVICEVIEW_H

#include <QtCore/QPointer>

#include <QtWidgets/QWidget>

class QMenu;

class ADeviceController;
class AImageWidget;

class ADeviceView : public QWidget {
    Q_OBJECT

    public:
        //! Constructor.
        explicit ADeviceView(QWidget *parent = NULL);

        //! Destructor.
        virtual ~ADeviceView() {}

        //! Get device controller.
        ADeviceController *controller() const;

        //! Set device controller.
        void setController(ADeviceController *ctrl);

    protected:
        //! Context menu event.
        virtual void contextMenuEvent(QContextMenuEvent *event);

        //! Resize event.
        virtual void resizeEvent(QResizeEvent *event);

        //! Event filter.
        virtual bool eventFilter(QObject *obj, QEvent *event);

    private:
        bool _action_wdg_animate_freezed;

        AImageWidget *_img_wdg;

        QMenu *_url_menu, *_dev_menu;

        QWidget *_action_wdg;

        QPointer<ADeviceController> _dev_ctrl;

        //! Create device action.
        void createDeviceAction(ADeviceController *ctrl);

        //! Create action widget.
        void createActionWidget();

        //! Animate action widget.
        void animateActionWidget(bool showed);

    private slots:
        //! Animate show action widget.
        void animateShowActionWidget();

        //! Animate hide action widget.
        void animateHideActionWidget();

};

#endif
