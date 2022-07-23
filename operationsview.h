#ifndef OPERATIONSVIEW_H
#define OPERATIONSVIEW_H

#include <QWidget>
#include <QTableView>
#include <QSqlRelationalTableModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

#include "filterswidget.h"

class OperationsView : public QWidget
{
    Q_OBJECT
public:
    OperationsView();
    ~OperationsView();

    void loadModel(QSqlRelationalTableModel * mod);
    QTableView* table();

public slots:
    void applyFilters(const QString &);
    void setBalance(double, double);

private:
    // Methodes


    // Attributs
    QVBoxLayout *mainLayout;
    QHBoxLayout *balanceLayout;
    QTableView *opsTable;

    QSqlRelationalTableModel * model;

    filtersWidget *filters;

    QLabel *qlTodayBalance;
    QLabel *qlFutureBalance;
};

#endif // OPERATIONSVIEW_H
