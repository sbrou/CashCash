#ifndef OPERATIONSVIEW_H
#define OPERATIONSVIEW_H

#include <QWidget>
#include <QTableView>
#include <QSqlRelationalTableModel>
#include <QVBoxLayout>

#include "filterswidget.h"

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
    QVBoxLayout *mainLayout;
    QTableView *opsTable;

    QSqlRelationalTableModel * model;

    filtersWidget *filters;
};

#endif // OPERATIONSVIEW_H
