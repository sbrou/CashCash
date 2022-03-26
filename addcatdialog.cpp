#include "addcatdialog.h"
#include "ui_addcatdialog.h"

#include <QPushButton>

void addCatDialog::init()
{
    ui->setupUi(this);
}

addCatDialog::addCatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addCatDialog),
    catId(-1)
{
    init();
}


addCatDialog::addCatDialog(int id, QSqlRelationalTableModel *model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addCatDialog)
{
    init();

    QPushButton *ok = ui->buttonBox->button(ui->buttonBox->Ok);
    QPushButton *cancel = ui->buttonBox->button(ui->buttonBox->Cancel);

    connect(ok, &QPushButton::clicked, this, &addCatDialog::submit);
    connect(cancel, &QPushButton::clicked, this, &addCatDialog::revert);

    catId = id;

    mapper = new QDataWidgetMapper(this);
    mapper->setModel(model);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->setItemDelegate(new QSqlRelationalDelegate(mapper));
    mapper->addMapping(ui->qleTitle, 1);
    mapper->setCurrentIndex(id);
}

void addCatDialog::revert()
{
    mapper->revert();
}

void addCatDialog::submit()
{
    mapper->submit();
    mapper->setCurrentIndex(catId);
}

//addCatDialog::addCatDialog(QWidget *parent) :
//    QDialog(parent),
//    ui(new Ui::addCatDialog)
//{
//    ui->setupUi(this);

//    ui->qcbType->addItem("DEPENSES",Category::SPENDING);
//    ui->qcbType->addItem("REVENUS",Category::INCOME);
//    ui->qcbType->addItem("EPARGNE",Category::SAVING);
//}

addCatDialog::~addCatDialog()
{
    delete ui;
}

QString addCatDialog::title()
{
    return ui->qleTitle->text();
}

