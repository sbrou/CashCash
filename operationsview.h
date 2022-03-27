#ifndef OPERATIONSVIEW_H
#define OPERATIONSVIEW_H

#include <QWidget>
#include <QTableView>
#include <QSqlRelationalTableModel>
#include <QHBoxLayout>

class OperationsView : public QWidget
{
    Q_OBJECT
public:
    OperationsView();

    void loadModel(QSqlRelationalTableModel * mod);
    QTableView* table();
private:
    // Methodes


    // Attributs
    QHBoxLayout *mainLayout;
    QTableView *opsTable;

    QSqlRelationalTableModel * model;
};

#endif // OPERATIONSVIEW_H
