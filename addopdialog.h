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
    explicit AddOpDialog(QStringList categories, QStringList tags, QWidget *parent = nullptr);
    ~AddOpDialog();

    QDate date();
    double amount();
    int category();
    int tag();
    QString description();

    void setDate(QDate);
    void setAmount(double);
    void setCategory(int);
    void setTag(int);
    void setDescription(const QString &);

private:
    Ui::AddOpDialog *ui;
};

#endif // ADDOPDIALOG_H
