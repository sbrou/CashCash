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

    ui->scrollArea->setVisible(false);
    setWindowTitle(QString("A propos de %1").arg(QCoreApplication::applicationName()));

    QPushButton *qpbCredits = new QPushButton(tr("Crédits"));
    qpbCredits->setCheckable(true);
    qpbCredits->setChecked(false);
    connect(qpbCredits, SIGNAL(toggled(bool)), ui->scrollArea, SLOT(setVisible(bool)));
    ui->buttonBox->addButton(qpbCredits, QDialogButtonBox::ActionRole);

    QString head = QString("<b>%1 version %2</b>").arg(QCoreApplication::applicationName()).arg(QCoreApplication::applicationVersion());
    QString qt_version = tr("Dévelopée avec ")+ QString("Qt %1").arg(QLibraryInfo::version().toString());
    QString app_sources = QString("<a href=\"https://github.com/sbrou/budget\"> %1 </a>").arg(tr("Code source"));
    ui->qlBody->setText(head + "<br>" + qt_version + "<br><br>" + app_sources);

    QString tab("&nbsp;");
    QString credits = tr("Développée par : ") + "Sopie Brou" + "<br>" +
            "<br>" + tr("Icons  créées par : ") + "<br>" +
            "<br>" + tab + "<a href=\"https://www.flaticon.com/free-icon/euro_1490850\" title=\"euro icon\">Euro icon created by Freepik - Flaticon</a>" +
            "<br>" + tab + "<a href=\"https://www.flaticon.com/free-icon/analysis_2041643\" title=\"analysis icon\">Analysis icon created by Freepik - Flaticon</a>" +
            "<br>" + tab + "<a href=\"https://www.flaticon.com/free-icon/calendar_4320177\" title=\"calendar icons\">Calendar icon created by DinosoftLabs - Flaticon</a>" +
            "<br>" + tab + "<a href=\"https://www.flaticon.com/free-icon/cancel_753345\" title=\"cancel icons\">Cancel icon created by Vectors Market - Flaticon</a>" +
            "<br>" + tab + "<a href=\"https://www.flaticon.com/free-icon/shape_3894687\" title=\"shape icons\">Shape icon created by Freepik - Flaticon</a>" +
            "<br>" + tab + "<a href=\"https://www.flaticon.com/free-icon/down_4212336\" title=\"down icons\">Down icon created by Ilham Fitrotul Hayat - Flaticon</a>" +
            "<br>" + tab + "<a href=\"https://www.flaticon.com/free-icon/euro_3675148\" title=\"euro icons\">Euro icon created by IconKanan - Flaticon</a>" +
            "<br>" + tab + "<a href=\"https://www.flaticon.com/free-icon/filter_8467740\" title=\"filter icons\">Filter icon created by icon_small - Flaticon</a>" +
            "<br>" + tab + "<a href=\"https://www.flaticon.com/free-icon/minus_148765\" title=\"minus icons\">Minus icon created by Smashicons - Flaticon</a>" +
            "<br>" + tab + "<a href=\"https://www.flaticon.com/free-icon/add_8156422\" title=\"new file icons\">Add icon created by Creatype - Flaticon</a>" +
            "<br>" + tab + "<a href=\"https://www.flaticon.com/free-icon/file-storage_3616558\" title=\"folder icons\">File storage icon created by DinosoftLabs - Flaticon</a>" +
            "<br>" + tab + "<a href=\"https://www.flaticon.com/free-icon/pencil_2280557\" title=\"pencil icons\">Pencil icon created by Freepik - Flaticon</a>" +
            "<br>" + tab + "<a href=\"https://www.flaticon.com/free-icon/plus_148764\" title=\"google plus icons\">Google plus icon created by Smashicons - Flaticon</a>" +
            "<br>" + tab + "<a href=\"https://www.flaticon.com/free-icon/redo_4706652\" title=\"reset icons\">Redo icon created by itim2101 - Flaticon</a>" +
            "<br>" + tab + "<a href=\"https://www.flaticon.com/free-icon/ruler_1151014\" title=\"ruler icons\">Ruler icon created by Smashicons - Flaticon</a>" +
            "<br>" + tab + "<a href=\"https://www.flaticon.com/free-icon/save-file_4725444\" title=\"save file icons\">Save file icon created by berkahicon - Flaticon</a>" +
            "<br>" + tab + "<a href=\"https://www.flaticon.com/free-icon/search_954591\" title=\"search icons\">Search icon created by Maxim Basinski Premium - Flaticon</a>" +
            "<br>" + tab + "<a href=\"https://www.flaticon.com/free-icon/label_4305629\" title=\"price tag icons\">Label icon created by bqlqn - Flaticon</a>" +
            "<br>" + tab + "<a href=\"https://www.flaticon.com/free-icon/target_610064\" title=\"target icons\">Target icon created by Vectors Market - Flaticon</a>" +
            "<br>" + tab + "<a href=\"https://www.flaticon.com/free-icon/up-arrow_4212335\" title=\"ui icons\">Up arrow icon created by Ilham Fitrotul Hayat - Flaticon</a>" +
            "<br>" + tab + "<a href=\"https://www.flaticon.com/free-icon/file_1091227\" title=\"upload icons\">Upload file icon created by Kiranshastry - Flaticon</a>"
            ;

    ui->qlCredits->setText(credits);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
