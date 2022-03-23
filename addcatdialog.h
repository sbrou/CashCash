#ifndef ADDCATDIALOG_H
#define ADDCATDIALOG_H

#include <QDialog>
#include <QtSql>
#include <QDataWidgetMapper>

#include "category.h"

namespace Ui {
class addCatDialog;
}

class addCatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit addCatDialog(QWidget *parent = nullptr);
    explicit addCatDialog(int id, QSqlRelationalTableModel *model, QWidget *parent = nullptr);
    ~addCatDialog();

    QString title();
    Category::Type type();

private slots:
    void revert();
    void submit();

private:
    void init();

    Ui::addCatDialog *ui;
    QDataWidgetMapper *mapper = nullptr;
    int catId;
};

#endif // ADDCATDIALOG_H
