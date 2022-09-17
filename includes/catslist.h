#ifndef CATSLIST_H
#define CATSLIST_H

#include <QDialog>
#include <QGridLayout>
#include <QListWidget>
#include <QPushButton>
#include <QSqlTableModel>
#include <QMenu>

#include "utilities.h"
#include "defines.h"

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
    void updateActions(const QItemSelection &selected);

signals:
    void commit();

protected slots:
    void applyAction(Action);
    void add();
    void edit();
    void remove();

private:
    Ui::GroupList *ui;
    Utilities::ToolBar *toolBar;
    QSqlTableModel *model;
    QMenu *contextMenu;
    GroupType groupType;
};

#endif // CATSLIST_H
