#ifndef NEWACCOUNTDIALOG_H
#define NEWACCOUNTDIALOG_H

#include <QDialog>

namespace Ui {
class NewAccountDialog;
}

class NewAccountDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewAccountDialog(QWidget *parent = nullptr);
    ~NewAccountDialog();

    QString title();
    double balance();

private:
    Ui::NewAccountDialog *ui;
};

#endif // NEWACCOUNTDIALOG_H
