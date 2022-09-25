#include "mainwindow.h"
#include "welcomedialog.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!QSqlDatabase::drivers().contains("QSQLITE"))
    {
        QMessageBox::critical(NULL,
                    "Unable to load database",
                    "This application needs the SQLITE driver"
                    );
        a.quit();
    }

    QTranslator qtTranslator;
    if (qtTranslator.load(QLocale::French,
                "qt", "_",
                QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
    {
        a.installTranslator(&qtTranslator);
    }

    QCoreApplication::setOrganizationName("SopiePersonalProjects");
    QCoreApplication::setApplicationName("MoulagApp");
    QCoreApplication::setApplicationVersion("1.0.0");

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "budget_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    a.setStyleSheet("QDateEdit::drop-down {  image: url(:/images/images/calendar.png);  }");

    MainWindow w;
    w.show();
    w.showWelcomeDialog();
    return a.exec();
}
