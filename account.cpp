#include "account.h"
#include "ui_account.h"

#include <QFileDialog>
#include <QDebug>

Account::Account(QString title, QWidget *parent)
    : QWidget{parent}
    , ui(new Ui::Account)
    , _locale(QLocale::German)
    , _title(title)
{
    ui->setupUi(this);

    opsModel = new OperationsTableModel(this);
    ui->qtvOps->setModel(opsModel);
    ui->qtvOps->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->qtvOps->horizontalHeader()->setStretchLastSection(true);
    ui->qtvOps->verticalHeader()->hide();
    ui->qtvOps->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->qtvOps->setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect(ui->qpbImportFile, SIGNAL(clicked()), this, SLOT(importFile()));

    setStandardRules();

//    show();
}

void Account::importFile()
{
    QString filename = QFileDialog::getOpenFileName(nullptr,tr("Importer fichier"), QDir::home().dirName(), tr("csv files (*.csv)"));
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        process_line(line);
    }

//    for (auto op = ops.begin(); op!= ops.end(); ++op)
//        addOperation(op->date(), op->category(), op->amount(), op->description());
}

void Account::process_line(QString line)
{
    QStringList infos = line.split(QLatin1Char(';'));

    QStringList date = infos.at(0).split(QLatin1Char('/'));
    QDate op_date(date.at(2).toInt(),date.at(1).toInt(),date.at(0).toInt());
    QString cat = getOperationCategory(infos.at(1));

    // Reflechir a comment proprement appeler addOperation en dehors de la lecture du fichier
    addOperation(op_date, infos.at(1), _locale.toDouble(infos.at(2)), cat);
}

QString Account::getOperationCategory(const QString &des)
{
    for (auto rule = _rules.cbegin(); rule != _rules.cend(); ++rule)
    {
        if (des.contains(rule.key()))
            return rule.value();
    }
    return "";
}

void Account::addOperation(QDate date, const QString &des, double amount, const QString &cat)
{
    int pos = opsModel->rowCount();
    opsModel->insertRows(pos, 1, QModelIndex());

    QModelIndex index = opsModel->index(pos, 0, QModelIndex());
    opsModel->setData(index, date, Qt::EditRole);
    index = opsModel->index(pos, 1, QModelIndex());
    opsModel->setData(index, cat, Qt::EditRole);
    index = opsModel->index(pos, 2, QModelIndex());
    opsModel->setData(index, amount, Qt::EditRole);
    index = opsModel->index(pos, 3, QModelIndex());
    opsModel->setData(index, des, Qt::EditRole);
}

void Account::setStandardRules()
{
    _rules.insert("CARREFOUR","FOOD");
    _rules.insert("HOMESERVE","HOUSE");
    _rules.insert("PHARMACIE","HEALTH");
    _rules.insert("FNAC","HOBBIES");
    _rules.insert("Maxi Zoo","MAIKO");
    _rules.insert("CHANTURGUE","HOUSE");
    _rules.insert("TotalEnergies","HOUSE");
    _rules.insert("PROXISERV","HOUSE");
    _rules.insert("AVANSSUR","HOUSE");
    _rules.insert("JOINT","JOINT");
    _rules.insert("T2C","TRANSPORT");
    _rules.insert("FR5110011000207555808944J","SAVING");
    _rules.insert("BOUYGUES","SUBSCRIPTIONS");
    _rules.insert("Deezer","SUBSCRIPTIONS");
    _rules.insert("ASTEK","SALARY");
}
