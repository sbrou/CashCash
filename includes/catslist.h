#ifndef CATSLIST_H
#define CATSLIST_H

#include <QDialog>
#include <QGridLayout>
#include <QListWidget>
#include <QPushButton>
#include <QSqlRelationalTableModel>
#include <QMenu>

#include <defines.h>

namespace Ui {
class GroupList;
}

class GroupList : public QDialog
{
    Q_OBJECT

public:
    explicit GroupList(GroupType type, QSqlTableModel * mod, QWidget *parent = nullptr);

public slots:
    void customMenuRequested(QPoint pos);

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
    QMenu *contextMenu;
    GroupType groupType;
};

#endif // CATSLIST_H
