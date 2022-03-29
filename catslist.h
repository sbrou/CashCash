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
    explicit CatsList(QSqlRelationalTableModel * mod, QWidget *parent = nullptr);
    ~CatsList();

signals:

private slots:
    void addNewCategory();
    void editCategory();
    void removeCategories();

private:
    QSqlRelationalTableModel *model;

    QGridLayout *mainLayout;

    QListWidget *catsView;

    QPushButton *qpbAddNew;
    QPushButton *qpbEdit;
    QPushButton *qpbRemove;
};

#endif // CATSLIST_H
