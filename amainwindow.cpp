#include "amainwindow.h"
#include "aservicecontroller.h"
#include "adeviceview.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
AMainWindow::AMainWindow(QWidget *parent) : QMainWindow(parent) {
    setCentralWidget(new ADeviceView(this));
}
