#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <QPushButton>
#include <QCoreApplication>
#include <QLibraryInfo>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    ui->qlCredits->setVisible(false);

    QPushButton *qpbCredits = new QPushButton(tr("Crédits"));
    qpbCredits->setCheckable(true);
    qpbCredits->setChecked(false);
    connect(qpbCredits, SIGNAL(toggled(bool)), ui->qlCredits, SLOT(setVisible(bool)));
    ui->buttonBox->addButton(qpbCredits, QDialogButtonBox::ActionRole);

    QString head = QString("<b>MoulagApp version %1</b>").arg(QCoreApplication::applicationVersion());
    QString qt_version = tr("Dévelopée avec ")+ QString("Qt %1").arg(QLibraryInfo::version().toString());
    QString app_sources = QString("<a href=\"https://github.com/sbrou/budget\"> %1 </a>").arg(tr("Code source"));
    ui->qlBody->setText(head + "<br>" + qt_version + "<br><br>" + app_sources);

    QString tab("&nbsp;");
    QString credits = tr("Développée par : ") + "Sopie Brou" + "<br>" +
            "<br>" + tr("Icons  créées par : ") + "<br>" +
            tab + "<a href=\"https://www.flaticon.com/free-icons/euro\" title=\"euro icons\">Euro icons created by Freepik - Flaticon</a>";

    ui->qlCredits->setText(credits);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
