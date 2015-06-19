#ifndef ADEVICEVIEW_H
#define ADEVICEVIEW_H

#include <QtCore/QPointer>

#include <QtWidgets/QWidget>

class QActionGroup;
class QLCDNumber;
class QLabel;
class QMenu;

class ADeviceController;
class AMultiImageWidget;

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
        //! Resize event.
        virtual void resizeEvent(QResizeEvent *event);

        //! Event filter.
        virtual bool eventFilter(QObject *obj, QEvent *event);

    private:
        bool _action_wdg_animate_freezed;

        AMultiImageWidget *_img_wdg;

        QMenu *_url_menu, *_dev_menu, *_flt_menu;

        QActionGroup *_url_act_grp, *_dev_act_grp, *_flt_act_grp;

        QLabel *_title_label;

        QWidget *_action_wdg;

        QLCDNumber *_capture_fps_lcd, *_preview_fps_lcd;

        QPointer<ADeviceController> _dev_ctrl;

        //! Create device action.
        void createDeviceAction(ADeviceController *ctrl);

        //! Create filter group action.
        void createFilterGroupAction(int grp_i);

        //! Create title widget.
        void createTitleWidget();

        //! Animate title widget.
        void animateTitleWidget(bool showed);

        //! Create action widget.
        void createActionWidget();

        //! Animate action widget.
        void animateActionWidget(bool showed);

    private slots:
        //! Animate show title widget.
        void animateShowTitleWidget();

        //! Animate hide title widget.
        void animateHideTitleWidget();

        //! Animate show action widget.
        void animateShowActionWidget();

        //! Animate hide action widget.
        void animateHideActionWidget();

        //! On capture fps changed.
        void onCaptureFpsChanged(double fps);

        //! On preview fps changed.
        void onPreviewFpsChanged(double fps);

};

#endif
