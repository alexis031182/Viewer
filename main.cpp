#include <QtCore/QTranslator>
#include <QtCore/QTextCodec>
#include <QtCore/QLocale>

#include <QtWidgets/QApplication>

#include "aservicecontroller.h"
#include "amainwindow.h"

// ========================================================================== //
//
// ========================================================================== //
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("AlexisSoft"));
    app.setOrganizationDomain(QStringLiteral("ru"));
    app.setApplicationName(QStringLiteral("Viewer"));
    app.setApplicationVersion(QStringLiteral("1.1"));

    AServiceController::instance();

    AMainWindow main_win;
    main_win.setWindowTitle(app.applicationDisplayName());
    main_win.resize(800,600);

    QMetaObject::invokeMethod(&main_win, "show", Qt::QueuedConnection);

    return app.exec();
}
