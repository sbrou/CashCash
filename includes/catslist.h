#ifndef CATSLIST_H
#define CATSLIST_H

#include <QDialog>
#include <QGridLayout>
#include <QListWidget>
#include <QPushButton>
#include <QSqlRelationalTableModel>

#include <defines.h>

namespace Ui {
class GroupList;
}

class GroupList : public QDialog
{
    Q_OBJECT

public:
    explicit GroupList(GroupType type, QSqlTableModel * mod, QWidget *parent = nullptr);

signals:
    void commit();

protected slots:
    void applyAction(Action);
    void add();
    void edit();
    void remove();

private:
    Ui::GroupList *ui;
    QSqlTableModel *model;
    GroupType groupType;
};

#endif // CATSLIST_H
