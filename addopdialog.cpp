#include "addopdialog.h"
#include "ui_addopdialog.h"

#include <QDoubleValidator>
#include <QPushButton>

void AddOpDialog::init()
{
    ui->setupUi(this);

    ui->qdeDate->setDate(QDate::currentDate());

    QDoubleValidator * amountValidator = new QDoubleValidator(ui->qleAmount);
    amountValidator->setLocale(QLocale::German);
    ui->qleAmount->setValidator(amountValidator);
}

AddOpDialog::AddOpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddOpDialog),
    opId(-1)
{
    init();
}


AddOpDialog::AddOpDialog(int id, QSqlRelationalTableModel *model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddOpDialog)
{
    init();

    QPushButton *ok = ui->buttonBox->button(ui->buttonBox->Ok);
    QPushButton *cancel = ui->buttonBox->button(ui->buttonBox->Cancel);

    connect(ok, &QPushButton::clicked, this, &AddOpDialog::submit);
    connect(cancel, &QPushButton::clicked, this, &AddOpDialog::revert);

//    int categoryIdx = model->fieldIndex("category");
//    qDebug() << model->fieldIndex("category");
//    int tagIdx = model->fieldIndex("tag");
//    qDebug() << model->fieldIndex("tag");
//    qDebug() << id;
//    ui->qcbCat->setModel(model->relationModel(2));
//    ui->qcbCat->setModelColumn(model->relationModel(2)->fieldIndex("name"));
//    ui->qcbTag->setModel(model->relationModel(4));
//    ui->qcbTag->setModelColumn(model->relationModel(tagIdx)->fieldIndex("name"));

//    qDebug() << "in AddOp " << model->record(opId);
//    cat_idx = model->record(opId).value("categories_name_2").toInt();
//    tag_idx = model->record(opId).value("name").toInt();

    opId = id;

    qDebug() << "in AddOpDIalog";
    QSqlQuery query("SELECT name FROM categories",model->database());
    while (query.next()) {
        qDebug() << query.value(0).toString();
    }

    qDebug() << model->fieldIndex("categories_name_2");

    mapper = new QDataWidgetMapper(this);
    mapper->setModel(model);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->setItemDelegate(new QSqlRelationalDelegate(this));
    mapper->addMapping(ui->qdeDate, 1);
    mapper->addMapping(ui->qleAmount, 3);
    mapper->addMapping(ui->qcbCat, model->fieldIndex("categories_name_2"));
    mapper->addMapping(ui->qcbTag, 4);
    mapper->addMapping(ui->qleDes, 5, "text");
    mapper->setCurrentIndex(id);
}

void AddOpDialog::revert()
{
    mapper->revert();
}

void AddOpDialog::submit()
{
    mapper->submit();
    mapper->setCurrentIndex(opId);
}

AddOpDialog::~AddOpDialog()
{
    delete ui;
    mapper->deleteLater();
//    model->deleteLater();
}

QDate AddOpDialog::date()
{
    return ui->qdeDate->date();
}

double AddOpDialog::amount()
{
    return ui->qleAmount->text().toDouble();
}

int AddOpDialog::category()
{
    return ui->qcbCat->currentIndex()+1;
}

int AddOpDialog::tag()
{
    return ui->qcbTag->currentIndex()+1;
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

void AddOpDialog::setCategory(int cat)
{
    ui->qcbCat->setCurrentIndex(cat);
}

void AddOpDialog::setTag(int tag)
{
    ui->qcbTag->setCurrentIndex(tag);
}

void AddOpDialog::setDescription(const QString& des)
{
    ui->qleDes->setText(des);
}

void AddOpDialog::fillCategories(QSqlTableModel * model, int field, const QString & cat)
{
    ui->qcbCat->setModel(model);
    ui->qcbCat->setModelColumn(field);
    if (!cat.isEmpty())
        ui->qcbCat->setCurrentText(cat);
}

void AddOpDialog::fillTags(QSqlTableModel * model , int field, const QString & tag)
{
    ui->qcbTag->setModel(model);
    ui->qcbTag->setModelColumn(field);
    if (!tag.isEmpty())
        ui->qcbTag->setCurrentText(tag);
}
