#include "groupinputdialog.h"
#include "ui_groupinputdialog.h"
#include "defines.h"

GroupInputDialog::GroupInputDialog(QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags),
      ui(new Ui::GroupInputDialog)
{
    ui->setupUi(this);
}

GroupInputDialog::~GroupInputDialog()
{
    delete ui;
}

int GroupInputDialog::getIndex(QWidget *parent, const QString &title, const QString &label,
                               QSqlTableModel *model, bool *ok, Qt::WindowFlags flags)
{
    GroupInputDialog* dialog(new GroupInputDialog(parent, flags));
    dialog->setWindowTitle(title);
    dialog->setLabelText(label);
    dialog->setComboBoxModel(model);

    const int ret = dialog->exec();
    if (ok)
        *ok = !!ret;
    if (ret) {
        return dialog->getComboBoxIndex();
    } else {
        return DEFAULT_GROUP;
    }
}

void GroupInputDialog::setLabelText(const QString & label)
{
    ui->label->setText(label+":");
}

void GroupInputDialog::setComboBoxModel(QSqlTableModel *model)
{
    ui->comboBox->setModel(model);
    ui->comboBox->setModelColumn(1);
}

int GroupInputDialog::getComboBoxIndex()
{
    return ui->comboBox->currentIndex() + 1;
}
