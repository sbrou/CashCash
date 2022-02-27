#include "addcatdialog.h"
#include "ui_addcatdialog.h"

addCatDialog::addCatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addCatDialog)
{
    ui->setupUi(this);

    ui->qcbType->addItem("DEPENSES",Category::SPENDING);
    ui->qcbType->addItem("REVENUS",Category::INCOME);
    ui->qcbType->addItem("EPARGNE",Category::SAVING);
}

addCatDialog::~addCatDialog()
{
    delete ui;
}

QString addCatDialog::title()
{
    return ui->qleTitle->text();
}

Category::Type addCatDialog::type()
{
    return static_cast<Category::Type>(ui->qcbType->currentData().toUInt());
}
