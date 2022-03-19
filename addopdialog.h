#ifndef ADDOPDIALOG_H
#define ADDOPDIALOG_H

#include <QDialog>
#include <QtSql>
#include <QDataWidgetMapper>

namespace Ui {
class AddOpDialog;
}

class AddOpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddOpDialog(int id, QSqlRelationalTableModel *model, QWidget *parent = nullptr);
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

    void fillCategories(QSqlTableModel * model, int field, const QString & cat);
    void fillTags(QSqlTableModel * model , int field, const QString & tag);

private slots:
    void revert();
    void submit();

private:
    Ui::AddOpDialog *ui;
    QDataWidgetMapper *mapper = nullptr;
    int opId;
};

#endif // ADDOPDIALOG_H
