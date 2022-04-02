#ifndef CATSLIST_H
#define CATSLIST_H

#include <QDialog>
#include <QGridLayout>
#include <QListWidget>
#include <QPushButton>

#include <QSqlRelationalTableModel>

class CatsList : public QDialog
{
    Q_OBJECT
public:
    explicit CatsList(QSqlTableModel * mod, QWidget *parent = nullptr);
    ~CatsList();

signals:
    void commit();

private slots:
    void addNewCategory();
    void editCategory();
    void removeCategories();

private:
    QSqlTableModel *model;

    QGridLayout *mainLayout;

    QListWidget *catsView;

    QPushButton *qpbAddNew;
    QPushButton *qpbEdit;
    QPushButton *qpbRemove;

    int _nb;
};

#endif // CATSLIST_H
