#include "goaldialog.h"
#include "ui_goaldialog.h"

GoalDialog::GoalDialog(QSqlTableModel * cats, QSqlTableModel * tags, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GoalDialog),
    catsModel(cats),
    tagsModel(tags)
{
    ui->setupUi(this);

    ui->horizontalLayout_2->setAlignment(ui->qrbCats, Qt::AlignCenter);
    ui->horizontalLayout_2->setAlignment(ui->qrbTags, Qt::AlignCenter);

    connect(ui->qrbCats, SIGNAL(toggled(bool)), this, SLOT(showCats(bool)));
    connect(ui->qrbTags, SIGNAL(toggled(bool)), this, SLOT(showTags(bool)));

    showGroup(CatType);
}

GoalDialog::~GoalDialog()
{
    delete ui;
}

void GoalDialog::showCats(bool checked)
{
    if (checked)
        showGroup(CatType);
}

void GoalDialog::showTags(bool checked)
{
    if (checked)
        showGroup(TagType);
}

void GoalDialog::showGroup(unsigned type)
{
    _goal.type = (GroupType) type;

    if (type == CatType) {
        ui->qcbName->setPlaceholderText(tr("Category"));
        ui->qcbName->setModel(catsModel);
    }
    else {
        ui->qcbName->setPlaceholderText(tr("Tag"));
        ui->qcbName->setModel(tagsModel);
    }
    ui->qcbName->setModelColumn(1);
    ui->qcbName->setCurrentIndex(-1);
}

Goal GoalDialog::goal()
{
    _goal.typeName = ui->qcbName->currentText();
    _goal.max = ui->qleAMount->text().toDouble();
    return _goal;
}
