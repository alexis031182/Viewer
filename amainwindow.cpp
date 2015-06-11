//BEGIN FOR TEST ONLY
#include "widgets/asourceselector.h"
//END FOR TEST ONLY

#include "amainwindow.h"
#include "aservicecontroller.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
AMainWindow::AMainWindow(QWidget *parent) : QMainWindow(parent) {
//BEGIN FOR TEST ONLY
    ASourceSelector *src_sel_wdg = new ASourceSelector(this);
    src_sel_wdg->setVideoDeviceModel(
        AServiceController::instance()->videoDeviceModel());
    setCentralWidget(src_sel_wdg);
//END FOR TEST ONLY
}
