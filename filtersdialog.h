#ifndef FILTERSDIALOG_H
#define FILTERSDIALOG_H

#include <QDialog>

namespace Ui {
class filtersDialog;
}

class filtersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit filtersDialog(QWidget *parent = nullptr);
    ~filtersDialog();

private:
    Ui::filtersDialog *ui;
};

#endif // FILTERSDIALOG_H
