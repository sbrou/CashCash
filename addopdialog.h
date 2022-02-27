#ifndef ADDOPDIALOG_H
#define ADDOPDIALOG_H

#include <QDialog>

namespace Ui {
class AddOpDialog;
}

class AddOpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddOpDialog(QStringList categories, QWidget *parent = nullptr);
    ~AddOpDialog();

    QDate date();
    double amount();
    QString category();
    QString description();

    void setDate(QDate);
    void setAmount(double);
    void setCategory(const QString &);
    void setDescription(const QString &);

private:
    Ui::AddOpDialog *ui;
};

#endif // ADDOPDIALOG_H
