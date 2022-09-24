#ifndef OPERATIONSVIEW_H
#define OPERATIONSVIEW_H

#include <QWidget>
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMenu>

#include "filterswidget.h"
#include "sqlrelationalmodel.h"

class OperationsView : public QWidget
{
    Q_OBJECT
public:
    OperationsView();
    ~OperationsView();

    void loadModel(SqlRelationalTableModel * mod);
    QTableView* table();
    QMenu *contextMenu();
    void saveSettings();
    void readSettings();

public slots:
    void customMenuRequested(QPoint pos);
    void applyFilters(const QString &);
    void setBalance(float, float);
    void resizeView();
    void resetView();

private slots:
    void slotShowFilters(bool isVisible);

private:
    // Methodes

    // Attributs
    QVBoxLayout *mainLayout;
    QHBoxLayout *balanceLayout;
    QTableView *opsTable;
    QMenu* cxtMenu;

    SqlRelationalTableModel * model;

    QPushButton *qpbHideOrShowFilters;
    filtersWidget *filters;

    QLabel *qlTodayBalance;
    QLabel *qlFutureBalance;
};

#endif // OPERATIONSVIEW_H
