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

int GroupInputDialog::getId(QWidget *parent, const QString &title, const QString &label,
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
        return dialog->getGroupId();
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

int GroupInputDialog::getGroupId()
{
    int row = ui->comboBox->currentIndex();
    QModelIndex idx = ui->comboBox->model()->index(row, 0); // first column : id_column
    QVariant data = ui->comboBox->model()->data(idx);
    return data.toInt();
}
