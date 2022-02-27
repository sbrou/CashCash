#ifndef ADDCATDIALOG_H
#define ADDCATDIALOG_H

#include <QDialog>

#include "category.h"

namespace Ui {
class addCatDialog;
}

class addCatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit addCatDialog(QWidget *parent = nullptr);
    ~addCatDialog();

    QString title();
    Category::Type type();

private:
    Ui::addCatDialog *ui;
};

#endif // ADDCATDIALOG_H
