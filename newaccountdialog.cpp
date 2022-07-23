#include "newaccountdialog.h"
#include "ui_newaccountdialog.h"

NewAccountDialog::NewAccountDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewAccountDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Créer un nouveau compte"));
}

NewAccountDialog::~NewAccountDialog()
{
    delete ui;
}

QString NewAccountDialog::title()
{
    return ui->qleTitle->text();
}

double NewAccountDialog::balance()
{
    return ui->qleBalance->text().toDouble();
}
