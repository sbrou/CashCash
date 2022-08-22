#ifndef OPERATIONSVIEW_H
#define OPERATIONSVIEW_H

#include <QWidget>
#include <QTableView>
#include <QSqlRelationalTableModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "filterswidget.h"

class OperationsView : public QWidget
{
    Q_OBJECT
public:
    OperationsView();
    ~OperationsView();

    void loadModel(QSqlRelationalTableModel * mod);
    QTableView* table();
    void saveSettings();
    void readSettings();

public slots:
    void applyFilters(const QString &);
    void setBalance(double, double);

private slots:
    void slotShowFilters(bool isVisible);

private:
    // Methodes

    // Attributs
    QVBoxLayout *mainLayout;
    QHBoxLayout *balanceLayout;
    QTableView *opsTable;

    QSqlRelationalTableModel * model;

    QPushButton *qpbHideOrShowFilters;
    filtersWidget *filters;

    QLabel *qlTodayBalance;
    QLabel *qlFutureBalance;
};

#endif // OPERATIONSVIEW_H
