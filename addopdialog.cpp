#include "addopdialog.h"
#include "ui_addopdialog.h"

#include <QDoubleValidator>

AddOpDialog::AddOpDialog(QStringList categories, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddOpDialog)
{
    ui->setupUi(this);

    QDoubleValidator * amountValidator = new QDoubleValidator(ui->qleAmount);
    amountValidator->setLocale(QLocale::German);
    ui->qleAmount->setValidator(amountValidator);

    ui->qcbCat->addItems(categories);
}

AddOpDialog::~AddOpDialog()
{
    delete ui;
}

QDate AddOpDialog::date()
{
    return ui->qdeDate->date();
}

double AddOpDialog::amount()
{
    return ui->qleAmount->text().toDouble();
}

QString AddOpDialog::category()
{
    return ui->qcbCat->currentText();
}

QString AddOpDialog::description()
{
    return ui->qleDes->text();
}

void AddOpDialog::setDate(QDate date)
{
    ui->qdeDate->setDate(date);
}

void AddOpDialog::setAmount(double amount)
{
    ui->qleAmount->setText(QString::number(amount));
}

void AddOpDialog::setCategory(const QString& cat)
{
    int index = ui->qcbCat->findText(cat);
    if (index != -1)
        ui->qcbCat->setCurrentIndex(index);
}

void AddOpDialog::setDescription(const QString& des)
{
    ui->qleDes->setText(des);
}
