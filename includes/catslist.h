#ifndef CATSLIST_H
#define CATSLIST_H

#include <QDialog>
#include <QGridLayout>
#include <QListWidget>
#include <QPushButton>
#include <QSqlRelationalTableModel>

#include <defines.h>

class GroupList : public QDialog
{
    Q_OBJECT

public:
    explicit GroupList(GroupType type, QSqlTableModel * mod, QWidget *parent = nullptr);
    ~GroupList();

signals:
    void commit();

protected slots:
    void add();
    void edit();
    void remove();

protected:
    QPushButton *qpbAddNew;
    QPushButton *qpbEdit;
    QPushButton *qpbRemove;

private:
    QSqlTableModel *model;
    QGridLayout *mainLayout;
    QListWidget *catsView;
};


//// CatsList

class CatsList : public GroupList
{
public:
    explicit CatsList(QSqlTableModel * mod, QWidget *parent = nullptr);
};

//// TagsList

class TagsList : public GroupList
{
public:
    explicit TagsList(QSqlTableModel * mod, QWidget *parent = nullptr);
};

#endif // CATSLIST_H
