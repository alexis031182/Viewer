#include <QtCore/QTranslator>
#include <QtCore/QTextCodec>
#include <QtCore/QLocale>

#include <QtWidgets/QApplication>

#include "amainwindow.h"

// ========================================================================== //
//
// ========================================================================== //
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("AlexisSoft"));
    app.setOrganizationDomain(QStringLiteral("ru"));
    app.setApplicationName(QStringLiteral("Viewer"));
    app.setApplicationVersion(QStringLiteral("1.0"));
    app.setQuitOnLastWindowClosed(false);

    AMainWindow main_win;
    main_win.setWindowTitle(app.applicationDisplayName());
    QMetaObject::invokeMethod(&main_win, "show", Qt::QueuedConnection);

    return app.exec();
}
