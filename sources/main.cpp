#include "mainwindow.h"
#include "welcomedialog.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("SopiePersonalProjects");
    QCoreApplication::setApplicationName("MoulagApp");
    QCoreApplication::setApplicationVersion("0.0.0");

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "budget_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    a.setStyleSheet("QDateEdit::drop-down {  image: url(:/images/images/calendar_48px.png);  }");

    MainWindow w;
    w.show();
    w.showWelcomeDialog();
    return a.exec();
}