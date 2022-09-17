#include "rulesdialog.h"
#include "ui_rulesdialog.h"

RulesDialog::RulesDialog(QSqlTableModel * mod_cats, QSqlTableModel * mod_tags, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RulesDialog)
{
    ui->setupUi(this);

    ui->qcbCat->setModel(mod_cats);
    ui->qcbCat->setModelColumn(1);

    ui->qcbTag->setModel(mod_tags);
    ui->qcbTag->setModelColumn(1);
}

RulesDialog::~RulesDialog()
{
    delete ui;
}

QString RulesDialog::expression() const
{
    return ui->qleExpression->text();
}

int RulesDialog::categoryId() const
{
    return ui->qcbCat->currentIndex() + 1;
}

QString RulesDialog::categoryName() const
{
    return ui->qcbCat->currentText();
}

int RulesDialog::tagId() const
{
    return ui->qcbTag->currentIndex() + 1;
}

QString RulesDialog::tagName() const
{
    return ui->qcbTag->currentText();
}

bool RulesDialog::isCaseSensitive() const
{
    return ui->qcbCasse->isChecked();
}

void RulesDialog::setExpression(const QString & expression, bool isCaseSensitive)
{
    ui->qleExpression->setText(expression);
    ui->qcbCasse->setChecked(isCaseSensitive);
}

void RulesDialog::setCategory(int id)
{
    ui->qcbCat->setCurrentIndex(id-1);
}

void RulesDialog::setTag(int id)
{
    ui->qcbTag->setCurrentIndex(id-1);
}
