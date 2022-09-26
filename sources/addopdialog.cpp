#include "addopdialog.h"
#include "ui_addopdialog.h"

#include <QDoubleValidator>
#include <QPushButton>

#include <defines.h>

void AddOpDialog::init(QSqlTableModel * cats, QSqlTableModel * tags)
{
    ui->setupUi(this);

    ui->qdeDate->setDate(QDate::currentDate());

    QDoubleValidator * amountValidator = new QDoubleValidator(ui->qleAmount);
    amountValidator->setLocale(QLocale::German);
    ui->qleAmount->setValidator(amountValidator);
    ui->qleAmount->addAction(QIcon(":/images/images/euro.png"), QLineEdit::TrailingPosition);

    ui->qcbCat->setModel(cats);
    ui->qcbCat->setModelColumn(1);

    ui->qcbTag->setModel(tags);
    ui->qcbTag->setModelColumn(1);
}

AddOpDialog::AddOpDialog(QSqlTableModel * cats, QSqlTableModel * tags, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddOpDialog),
    opId(-1)
{
    init(cats, tags);
}


AddOpDialog::AddOpDialog(int id, SqlRelationalTableModel *model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddOpDialog),
    opId(id)
{
    init(model->relationModel(CatIndex), model->relationModel(TagIndex));

    QPushButton *ok = ui->buttonBox->button(ui->buttonBox->Ok);
    QPushButton *cancel = ui->buttonBox->button(ui->buttonBox->Cancel);

    connect(ok, &QPushButton::clicked, this, &AddOpDialog::submit);
    connect(cancel, &QPushButton::clicked, this, &AddOpDialog::revert);

    mapper = new QDataWidgetMapper(this);
    mapper->setModel(model);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->setItemDelegate(new QSqlRelationalDelegate(this));
    mapper->addMapping(ui->qdeDate, DateIndex);
    mapper->addMapping(ui->qleAmount, AmountIndex);
    mapper->addMapping(ui->qcbCat, CatIndex);
    mapper->addMapping(ui->qcbTag, TagIndex);
    mapper->addMapping(ui->qleDes, DesIndex, "text");
    mapper->setCurrentIndex(id);

    ui->qleAmount->setText(QString::number(ui->qleAmount->text().toFloat()));
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
    if (mapper != nullptr)
        mapper->deleteLater();
}

QDate AddOpDialog::date()
{
    return ui->qdeDate->date();
}

float AddOpDialog::amount()
{
    return ui->qleAmount->text().toFloat();
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

void AddOpDialog::setAmount(float amount)
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


