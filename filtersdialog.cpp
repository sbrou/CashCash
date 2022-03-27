#include "filtersdialog.h"
#include "ui_filtersdialog.h"

filtersDialog::filtersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::filtersDialog)
{
    ui->setupUi(this);
}

filtersDialog::~filtersDialog()
{
    delete ui;
}
