#include "goaldialog.h"
#include "ui_goaldialog.h"

#include <QMessageBox>

GoalDialog::GoalDialog(QSqlTableModel * cats, QSqlTableModel * tags, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GoalDialog),
    catsModel(cats),
    tagsModel(tags)
{
    ui->setupUi(this);

    ui->horizontalLayout_2->setAlignment(ui->qrbCats, Qt::AlignCenter);
    ui->horizontalLayout_2->setAlignment(ui->qrbTags, Qt::AlignCenter);

    QDoubleValidator * amValidator = new QDoubleValidator(ui->qleAMount);
    amValidator->setLocale(QLocale::German);
    amValidator->setBottom(0);
    ui->qleAMount->setValidator(amValidator);
    ui->qleAMount->addAction(QIcon(":/images/images/euro_48px.png"), QLineEdit::TrailingPosition);

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
    _goal.typeId = ui->qcbName->currentIndex()+1;
    _goal.max = ui->qleAMount->text().toFloat();
    return _goal;
}

void GoalDialog::accept()
{
    bool ok;
    ui->qleAMount->text().toFloat(&ok);
    if (ui->qcbName->currentIndex() < 0) {
        QMessageBox::critical(this, tr("Erreur"), tr("Veuillez sélectionner une catégorie ou un tag."));
        setResult(QDialog::Rejected);
        return;
    }
    else if (!ok) {
        QMessageBox::critical(this, tr("Erreur"), tr("Veuillez entrer un montant."));
        setResult(QDialog::Rejected);
        return;
    }
    else
        QDialog::accept();
}
