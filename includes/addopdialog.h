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
    explicit AddOpDialog(QSqlTableModel * cats, QSqlTableModel * tags, QWidget *parent = nullptr);
    explicit AddOpDialog(int id, QSqlRelationalTableModel *model, QWidget *parent = nullptr);
    ~AddOpDialog();

    QDate date();
    float amount();
    int category();
    int tag();
    QString description();

    void setDate(QDate);
    void setAmount(float);
    void setCategory(int);
    void setTag(int);
    void setDescription(const QString &);

private slots:
    void revert();
    void submit();

private:
    void init(QSqlTableModel * cats, QSqlTableModel * tags);

    Ui::AddOpDialog *ui;
    QDataWidgetMapper *mapper = nullptr;
    int opId;
};

#endif // ADDOPDIALOG_H
